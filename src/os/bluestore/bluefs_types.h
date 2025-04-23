// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#ifndef CEPH_OS_BLUESTORE_BLUEFS_TYPES_H
#define CEPH_OS_BLUESTORE_BLUEFS_TYPES_H

#include <optional>

#include "bluestore_types.h"
#include "include/denc.h"
#include "include/encoding.h"
#include "include/utime.h"

class bluefs_extent_t {
   public:
    uint64_t offset = 0;
    uint32_t length = 0;
    uint8_t bdev;

    bluefs_extent_t(uint8_t b = 0, uint64_t o = 0, uint32_t l = 0) : offset(o), length(l), bdev(b) {}

    uint64_t end() const { return offset + length; }
    DENC(bluefs_extent_t, v, p) {
        DENC_START(1, 1, p);
        denc_lba(v.offset, p);
        denc_varint_lowz(v.length, p);
        denc(v.bdev, p);
        DENC_FINISH(p);
    }

    void dump(ceph::Formatter* f) const;
    static void generate_test_instances(std::list<bluefs_extent_t*>&);
};
WRITE_CLASS_DENC(bluefs_extent_t)

std::ostream& operator<<(std::ostream& out, const bluefs_extent_t& e);

struct bluefs_fnode_delta_t {
    uint64_t ino;
    uint64_t size;
    utime_t mtime;
    uint64_t offset;  // Contains offset in file of extents.
                      // Equal to 'allocated' when created.
                      // Used for consistency checking.
    mempool::bluefs::vector<bluefs_extent_t> extents;

    DENC(bluefs_fnode_delta_t, v, p) {
        DENC_START(1, 1, p);
        denc_varint(v.ino, p);
        denc_varint(v.size, p);
        denc(v.mtime, p);
        denc(v.offset, p);
        denc(v.extents, p);
        DENC_FINISH(p);
    }
};
WRITE_CLASS_DENC(bluefs_fnode_delta_t)

std::ostream& operator<<(std::ostream& out, const bluefs_fnode_delta_t& delta);

struct bluefs_fnode_t {
    uint64_t ino;
    uint64_t size;
    utime_t mtime;
    uint8_t __unused__;  // was prefer_bdev
    mempool::bluefs::vector<bluefs_extent_t> extents;

    // precalculated logical offsets for extents vector entries
    // allows fast lookup for extent index by the offset value via upper_bound()
    mempool::bluefs::vector<uint64_t> extents_index;

    uint64_t allocated;
    uint64_t allocated_commited;

    bluefs_fnode_t() : ino(0), size(0), __unused__(0), allocated(0), allocated_commited(0) {}

    uint64_t get_allocated() const { return allocated; }

    void recalc_allocated() {
        allocated = 0;
        extents_index.reserve(extents.size());
        for (auto& p : extents) {
            extents_index.emplace_back(allocated);
            allocated += p.length;
        }
        allocated_commited = allocated;
    }

    DENC_HELPERS
    void bound_encode(size_t& p) const { _denc_friend(*this, p); }
    void encode(ceph::buffer::list::contiguous_appender& p) const {
        DENC_DUMP_PRE(bluefs_fnode_t);
        _denc_friend(*this, p);
    }
    void decode(ceph::buffer::ptr::const_iterator& p) {
        _denc_friend(*this, p);
        recalc_allocated();
    }
    template <typename T, typename P>
    friend std::enable_if_t<std::is_same_v<bluefs_fnode_t, std::remove_const_t<T>>> _denc_friend(T& v, P& p) {
        DENC_START(1, 1, p);
        denc_varint(v.ino, p);
        denc_varint(v.size, p);
        denc(v.mtime, p);
        denc(v.__unused__, p);
        denc(v.extents, p);
        DENC_FINISH(p);
    }

    void reset_delta() { allocated_commited = allocated; }
    void claim_extents(mempool::bluefs::vector<bluefs_extent_t>& extents) {
        for (const auto& p : extents) {
            append_extent(p);
        }
        extents.clear();
    }
    void append_extent(const bluefs_extent_t& ext) {
        if (!extents.empty() && extents.back().end() == ext.offset && extents.back().bdev == ext.bdev && (uint64_t)extents.back().length + (uint64_t)ext.length < 0xffffffff) {
            extents.back().length += ext.length;
        } else {
            extents_index.emplace_back(allocated);
            extents.push_back(ext);
        }
        allocated += ext.length;
    }

    void pop_front_extent() {
        auto it = extents.begin();
        allocated -= it->length;
        extents_index.erase(extents_index.begin());
        for (auto& i : extents_index) {
            i -= it->length;
        }
        extents.erase(it);
    }

    void swap_extents(bluefs_fnode_t& other) {
        other.extents.swap(extents);
        other.extents_index.swap(extents_index);
        std::swap(allocated, other.allocated);
        std::swap(allocated_commited, other.allocated_commited);
    }
    void clear_extents() {
        extents_index.clear();
        extents.clear();
        allocated = 0;
        allocated_commited = 0;
    }

    mempool::bluefs::vector<bluefs_extent_t>::iterator seek(uint64_t off, uint64_t* x_off);
    bluefs_fnode_delta_t* make_delta(bluefs_fnode_delta_t* delta);

    void dump(ceph::Formatter* f) const;
    static void generate_test_instances(std::list<bluefs_fnode_t*>& ls);
};
WRITE_CLASS_DENC(bluefs_fnode_t)

std::ostream& operator<<(std::ostream& out, const bluefs_fnode_t& file);

/**
 * @brief BlueFS文件系统布局结构体
 *
 * 该结构定义了BlueFS文件系统使用的设备布局配置信息。BlueFS是Ceph BlueStore的一部分，
 * 负责管理元数据和日志的存储。
 *
 * @details BlueFS可以使用一到三个设备:
 *   - 共享设备: 与BlueStore共享的主要存储设备
 *   - 专用数据库设备(可选): 用于存储元数据
 *   - 专用WAL设备(可选): 用于写前日志(Write-Ahead Log)
 *
 * 应用场景:
 *   - 初始化BlueFS时指定可用设备配置
 *   - 在BlueFS操作过程中查询当前使用的设备布局
 *   - 序列化/反序列化布局信息用于持久化存储
 *
 * 成员变量:
 *   @var shared_bdev    与BlueStore共享的设备索引
 *   @var dedicated_db   是否存在专用的block.db设备
 *   @var dedicated_wal  是否存在专用的block.wal设备
 *
 * 主要方法:
 *   - single_shared_device(): 检查是否只使用单一共享设备
 *   - operator==: 比较两个布局结构是否相同
 *   - encode/decode: 序列化和反序列化布局信息
 *   - dump: 将布局信息格式化输出到指定格式
 */
struct bluefs_layout_t {
    unsigned shared_bdev = 0;    ///< which bluefs bdev we are sharing
    bool dedicated_db = false;   ///< whether block.db is present
    bool dedicated_wal = false;  ///< whether block.wal is present

    bool single_shared_device() const { return !dedicated_db && !dedicated_wal; }

    bool operator==(const bluefs_layout_t& other) const { return shared_bdev == other.shared_bdev && dedicated_db == other.dedicated_db && dedicated_wal == other.dedicated_wal; }

    void encode(ceph::buffer::list& bl) const;
    void decode(ceph::buffer::list::const_iterator& p);
    void dump(ceph::Formatter* f) const;
};
WRITE_CLASS_ENCODER(bluefs_layout_t)

struct bluefs_super_t {
    uuid_d uuid;      ///< unique to this bluefs instance
    uuid_d osd_uuid;  ///< matches the osd that owns us
    uint64_t version;
    uint32_t block_size;

    bluefs_fnode_t log_fnode;

    std::optional<bluefs_layout_t> memorized_layout;

    bluefs_super_t() : version(0), block_size(4096) {}

    uint64_t block_mask() const { return ~((uint64_t)block_size - 1); }

    void encode(ceph::buffer::list& bl) const;
    void decode(ceph::buffer::list::const_iterator& p);
    void dump(ceph::Formatter* f) const;
    static void generate_test_instances(std::list<bluefs_super_t*>& ls);
};
WRITE_CLASS_ENCODER(bluefs_super_t)

std::ostream& operator<<(std::ostream&, const bluefs_super_t& s);

struct bluefs_transaction_t {
    typedef enum {
        OP_NONE = 0,
        OP_INIT,             ///< initial (empty) file system marker
        OP_ALLOC_ADD,        ///< OBSOLETE: add extent to available block storage (extent)
        OP_ALLOC_RM,         ///< OBSOLETE: remove extent from available block storage (extent)
        OP_DIR_LINK,         ///< (re)set a dir entry (dirname, filename, ino)
        OP_DIR_UNLINK,       ///< remove a dir entry (dirname, filename)
        OP_DIR_CREATE,       ///< create a dir (dirname)
        OP_DIR_REMOVE,       ///< remove a dir (dirname)
        OP_FILE_UPDATE,      ///< set/update file metadata (file)
        OP_FILE_REMOVE,      ///< remove file (ino)
        OP_JUMP,             ///< jump the seq # and offset
        OP_JUMP_SEQ,         ///< jump the seq #
        OP_FILE_UPDATE_INC,  ///< incremental update file metadata (file)
    } op_t;

    uuid_d uuid;               ///< fs uuid
    uint64_t seq;              ///< sequence number
    ceph::buffer::list op_bl;  ///< encoded transaction ops

    bluefs_transaction_t() : seq(0) {}

    /**
     * @brief 获取操作类型的名称字符串
     *
     * 根据给定的操作类型返回对应的人类可读名称
     *
     * @param op 操作类型枚举值
     * @return const char* 返回操作的名称字符串
     */
    static const char* get_op_name(__u8 op) {
        switch (op) {
            case OP_NONE:
                return "none";
            case OP_INIT:
                return "init";
            case OP_ALLOC_ADD:
                return "alloc_add";
            case OP_ALLOC_RM:
                return "alloc_rm";
            case OP_DIR_LINK:
                return "dir_link";
            case OP_DIR_UNLINK:
                return "dir_unlink";
            case OP_DIR_CREATE:
                return "dir_create";
            case OP_DIR_REMOVE:
                return "dir_remove";
            case OP_FILE_UPDATE:
                return "file_update";
            case OP_FILE_REMOVE:
                return "file_remove";
            case OP_JUMP:
                return "jump";
            case OP_JUMP_SEQ:
                return "jump_seq";
            case OP_FILE_UPDATE_INC:
                return "file_update_inc";
            default:
                return "unknown";
        }
    }

    void clear() { *this = bluefs_transaction_t(); }
    bool empty() const { return op_bl.length() == 0; }

    void op_init() {
        using ceph::encode;
        encode((__u8)OP_INIT, op_bl);
    }
    void op_dir_create(std::string_view dir) {
        using ceph::encode;
        encode((__u8)OP_DIR_CREATE, op_bl);
        encode(dir, op_bl);
    }
    void op_dir_remove(std::string_view dir) {
        using ceph::encode;
        encode((__u8)OP_DIR_REMOVE, op_bl);
        encode(dir, op_bl);
    }
    void op_dir_link(std::string_view dir, std::string_view file, uint64_t ino) {
        using ceph::encode;
        encode((__u8)OP_DIR_LINK, op_bl);
        encode(dir, op_bl);
        encode(file, op_bl);
        encode(ino, op_bl);
    }
    void op_dir_unlink(std::string_view dir, std::string_view file) {
        using ceph::encode;
        encode((__u8)OP_DIR_UNLINK, op_bl);
        encode(dir, op_bl);
        encode(file, op_bl);
    }
    void op_file_update(bluefs_fnode_t& file) {
        using ceph::encode;
        encode((__u8)OP_FILE_UPDATE, op_bl);
        encode(file, op_bl);
        file.reset_delta();
    }
    /* streams update to bufferlist and clears update state */
    void op_file_update_inc(bluefs_fnode_t& file) {
        using ceph::encode;
        bluefs_fnode_delta_t delta;
        file.make_delta(&delta);  // also resets delta to zero
        encode((__u8)OP_FILE_UPDATE_INC, op_bl);
        encode(delta, op_bl);
    }
    void op_file_remove(uint64_t ino) {
        using ceph::encode;
        encode((__u8)OP_FILE_REMOVE, op_bl);
        encode(ino, op_bl);
    }
    void op_jump(uint64_t next_seq, uint64_t offset) {
        using ceph::encode;
        encode((__u8)OP_JUMP, op_bl);
        encode(next_seq, op_bl);
        encode(offset, op_bl);
    }
    void op_jump_seq(uint64_t next_seq) {
        using ceph::encode;
        encode((__u8)OP_JUMP_SEQ, op_bl);
        encode(next_seq, op_bl);
    }
    void claim_ops(bluefs_transaction_t& from) { op_bl.claim_append(from.op_bl); }

    void encode(ceph::buffer::list& bl) const;
    void decode(ceph::buffer::list::const_iterator& p);
    void dump(ceph::Formatter* f) const;
    static void generate_test_instances(std::list<bluefs_transaction_t*>& ls);
};
WRITE_CLASS_ENCODER(bluefs_transaction_t)

std::ostream& operator<<(std::ostream& out, const bluefs_transaction_t& t);
#endif
