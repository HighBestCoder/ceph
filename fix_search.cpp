#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

constexpr size_t READ_BLOCK_SIZE = 4 * 1024 * 1024;
constexpr size_t PROCESS_BLOCK_SIZE = 4 * 1024;

std::string parse_uuid_to_bytes(const std::string& uuid_str) {
    std::string hex;
    hex.reserve(32);
    for (char c : uuid_str) {
        if (c != '-') {
            if (!std::isxdigit(static_cast<unsigned char>(c))) {
                throw std::invalid_argument("Invalid character in UUID");
            }
            hex += c;
        }
    }
    if (hex.size() != 32) {
        throw std::invalid_argument("UUID must have exactly 32 hex digits after removing dashes");
    }
    std::string result;
    result.reserve(16);
    for (size_t i = 0; i < hex.size(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byte_str, nullptr, 16));
        result.push_back(byte);
    }
    return result;
}

const void* fast_memmem(const void* haystack, size_t haystacklen, const void* needle, size_t needlelen) {
    // 检查是否足够大以包含偏移+目标长度
    if (needlelen == 0 || haystacklen < 6 + needlelen) {
        return nullptr;
    }

    const char* ptr = static_cast<const char*>(haystack);
    if (memcmp(ptr + 6, needle, needlelen) == 0) {
        return ptr + 6;  // 返回匹配位置的指针
    }

    return nullptr;
}

uint64_t parse_u64(const std::string& str) {
    try {
        size_t idx = 0;
        uint64_t val = std::stoull(str, &idx, 0);
        if (idx != str.size()) throw std::invalid_argument("Invalid characters after number");
        return val;
    } catch (...) {
        throw std::invalid_argument("Invalid uint64_t value: " + str);
    }
}

struct ReadRequest {
    int fd;
    uint64_t offset;
    uint64_t length;
    char* buffer;
};

// 分析线程 发请求 -> 读取线程
std::mutex read_mutex;
std::condition_variable read_cond;
std::queue<ReadRequest> read_queue;

struct ReadResult {
    int fd;
    uint64_t offset;
    uint64_t length;
    char* buffer;
    int64_t bytes_read;
};

// 读取线程 发结果 -> 分析线程
std::mutex result_mutex;
std::condition_variable result_cond;
std::queue<ReadResult> result_queue;

std::atomic<bool> stop_flag(false);

// 读取线程函数
void thd_reader(void) {
    while (!stop_flag) {
        ReadRequest req;
        {
            std::unique_lock<std::mutex> lock(read_mutex);
            read_cond.wait(lock, [] { return !read_queue.empty() || stop_flag; });
            if (stop_flag) break;
            req = read_queue.front();
            read_queue.pop();
        }

        ssize_t bytes_read = pread(req.fd, req.buffer, req.length, req.offset);
        if (bytes_read < 0) {
            perror("pread");
            stop_flag = true;
            {
                std::lock_guard<std::mutex> lock(result_mutex);
                result_queue.push({req.fd, req.offset, req.length, req.buffer, bytes_read});
            }
            break;
        }

        ReadResult res{req.fd, req.offset, req.length, req.buffer, bytes_read};
        {
            std::lock_guard<std::mutex> lock(result_mutex);
            result_queue.push(res);
        }
        result_cond.notify_one();
    }

    stop_flag = true;
}

// 分析线程函数
void thd_analyzer(int fd, uint64_t disk_offset, uint64_t length, std::string uuid_bytes) {
    uint64_t disk_end = disk_offset + length;

    // 这里先生成一些4K对齐的内存，每个都是READ_BLOCK_SIZE大小
    std::vector<char*> buffers_list;
    for (int i = 0; i < 4; i++) {
        char* buffer = nullptr;
        if (posix_memalign(reinterpret_cast<void**>(&buffer), READ_BLOCK_SIZE, READ_BLOCK_SIZE) != 0) {
            perror("posix_memalign");
            stop_flag = true;
            return;
        }
        buffers_list.push_back(buffer);
    }

    auto push_read_request = [&](void) {
        if (buffers_list.empty()) {
            std::cerr << "No available buffers for read request\n";
            return;
        }

        if (disk_offset >= disk_end) {
            std::cerr << "Disk offset out of range\n";
            return;
        }

        if (disk_offset + READ_BLOCK_SIZE > disk_end) {
            std::cerr << "Read request exceeds disk end\n";
            return;
        }
        // 从buffers_list中取出一个buffer
        char* buffer = buffers_list.back();
        buffers_list.pop_back();

        ReadRequest req;
        req.fd = fd;
        req.offset = disk_offset;
        req.length = READ_BLOCK_SIZE;
        req.buffer = buffer;

        {
            std::lock_guard<std::mutex> lock(read_mutex);
            read_queue.push(req);
        }
        read_cond.notify_one();
        disk_offset += READ_BLOCK_SIZE;
    };

    push_read_request();

    static auto start_time = std::chrono::steady_clock::now();
    static uint64_t total_bytes_processed = 0;
    int read_count = 1;

    // 从disk_offset开始读取数据
    while (!stop_flag && disk_offset < disk_end) {
        // 这里等待读取线程的结果
        ReadResult res;
        {
            std::unique_lock<std::mutex> lock(result_mutex);
            result_cond.wait(lock, [] { return !result_queue.empty() || stop_flag; });
            if (stop_flag) break;
            res = result_queue.front();
            result_queue.pop();
        }

        if (res.bytes_read < 0) {
            std::cerr << "Error reading from disk: " << res.bytes_read << "\n";
            break;
        }

        if (res.bytes_read == 0) {
            std::cout << "EOF reached\n";
            break;
        }

        // 在开始处理数据之前，先发起一个读取请求
        push_read_request();
        read_count++;

        // 处理数据
        size_t blocks = res.bytes_read / PROCESS_BLOCK_SIZE;
        for (size_t i = 0; i < blocks; ++i) {
            char* block = res.buffer + i * PROCESS_BLOCK_SIZE;
            const void* found = fast_memmem(block, PROCESS_BLOCK_SIZE, uuid_bytes.data(), uuid_bytes.size());
            if (found) {
                const char* found_ptr = static_cast<const char*>(found);
                off_t found_block_offset = res.offset + i * PROCESS_BLOCK_SIZE;
                off_t internal_offset = found_ptr - block;

                uint64_t seq = 0;
                if (found_ptr + uuid_bytes.size() + sizeof(seq) <= block + PROCESS_BLOCK_SIZE) {
                    std::memcpy(&seq, found_ptr + uuid_bytes.size(), sizeof(seq));
                } else {
                    std::cerr << "Found UUID at end of block, seq not available\n";
                    continue;
                }

                std::cout << "Found UUID at offset: " << found_block_offset << ", seq: " << seq << ", internal_offset: " << internal_offset << "\n";
            }
        }

        total_bytes_processed += READ_BLOCK_SIZE;
        double percent = (double)total_bytes_processed / length * 100.0;

        if (read_count % 1000 == 0) {
            auto now = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed = now - start_time;
            double elapsed_seconds = elapsed.count();

            double speed = total_bytes_processed / elapsed_seconds;  // bytes/sec
            double remaining_bytes = length - total_bytes_processed;
            double eta = remaining_bytes / speed;

            std::cerr << std::fixed << std::setprecision(2);
            std::cerr << "[Progress] " << percent << "% done, elapsed: " << elapsed_seconds << "s, ETA: " << eta << "s\n";
        }

        // 处理完数据后，释放buffer
        buffers_list.push_back(res.buffer);
    }

    // 清理剩余的buffer
    for (char* buffer : buffers_list) {
        free(buffer);
    }
    stop_flag = true;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " /dev/sdX <uuid> <offset> <length>\n";
        return 1;
    }

    std::string disk_path = argv[1];
    std::string uuid_bytes;
    try {
        uuid_bytes = parse_uuid_to_bytes(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing UUID: " << e.what() << "\n";
        return 1;
    }

    uint64_t start_offset = 0, total_length = 0;
    try {
        start_offset = parse_u64(argv[3]);
        total_length = parse_u64(argv[4]);
    } catch (const std::exception& e) {
        std::cerr << "Invalid offset/length: " << e.what() << "\n";
        return 1;
    }

    int fd = open(disk_path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open disk");
        return 1;
    }

    // 这里启动两个线程
    std::thread reader_thread(thd_reader);
    std::thread analyzer_thread(thd_analyzer, fd, start_offset, total_length, uuid_bytes);

    // 等待线程结束
    if (reader_thread.joinable()) {
        reader_thread.join();
    }

    if (analyzer_thread.joinable()) {
        analyzer_thread.join();
    }

    close(fd);
    return 0;
}
