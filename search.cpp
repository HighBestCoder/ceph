#include <fcntl.h>
#include <libaio.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <future>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

constexpr size_t READ_BLOCK_SIZE = 4 * 1024 * 1024;
constexpr size_t PROCESS_BLOCK_SIZE = 4 * 1024;
constexpr size_t NUM_THREADS = 256;

struct Result {
    off_t offset;
    uint64_t seq;
    off_t internal_offset;
};

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
    if (needlelen == 0 || haystacklen < needlelen) return nullptr;
    std::string_view hay(static_cast<const char*>(haystack), haystacklen);
    std::string_view nee(static_cast<const char*>(needle), needlelen);
    auto it = std::search(hay.begin(), hay.end(), std::boyer_moore_searcher(nee.begin(), nee.end()));
    if (it != hay.end()) {
        return static_cast<const void*>(&(*it));
    }
    return nullptr;
}

int64_t get_file_size(const std::string& path) {
    struct stat stat_buf;
    int64_t stat_size = -1;

    if (stat(path.c_str(), &stat_buf) == 0) {
        stat_size = stat_buf.st_size;
    }

    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return stat_size;
    }

    uint64_t ioctl_size = 0;
    if (ioctl(fd, BLKGETSIZE64, &ioctl_size) == 0) {
        close(fd);
        return std::max(stat_size, static_cast<int64_t>(ioctl_size));
    }

    close(fd);
    return stat_size;
}

void scan_range_libaio(const std::string& path, const std::string& uuid_bytes, off_t start_offset, off_t end_offset, std::vector<Result>& results, std::mutex& results_mutex) {
    int fd = open(path.c_str(), O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open (thread)");
        return;
    }

    io_context_t ctx = {};
    if (io_setup(1, &ctx) < 0) {
        perror("io_setup");
        close(fd);
        return;
    }

    void* buffer = nullptr;
    if (posix_memalign(&buffer, 4096, READ_BLOCK_SIZE) != 0) {
        perror("posix_memalign");
        io_destroy(ctx);
        close(fd);
        return;
    }

    for (off_t offset = start_offset; offset < end_offset; offset += READ_BLOCK_SIZE) {
        off_t chunk_size = std::min((off_t)READ_BLOCK_SIZE, end_offset - offset);

        struct iocb cb{}, *cbs[1] = {&cb};
        io_prep_pread(&cb, fd, buffer, chunk_size, offset);

        if (io_submit(ctx, 1, cbs) < 0) {
            perror("io_submit");
            continue;
        }

        struct io_event events[1];
        int ret = io_getevents(ctx, 1, 1, events, nullptr);
        if (ret < 0) {
            perror("io_getevents");
            continue;
        }

        ssize_t bytes_read = events[0].res;
        if (bytes_read <= 0) continue;

        size_t blocks = bytes_read / PROCESS_BLOCK_SIZE;
        for (size_t i = 0; i < blocks; ++i) {
            char* block = static_cast<char*>(buffer) + i * PROCESS_BLOCK_SIZE;
            const void* found = fast_memmem(block, PROCESS_BLOCK_SIZE, uuid_bytes.data(), uuid_bytes.size());
            if (found) {
                const char* found_ptr = static_cast<const char*>(found);
                if (found_ptr + uuid_bytes.size() + 8 <= block + PROCESS_BLOCK_SIZE) {
                    uint64_t seq;
                    std::memcpy(&seq, found_ptr + uuid_bytes.size(), sizeof(seq));
                    Result res{static_cast<off_t>(offset + static_cast<off_t>(i * PROCESS_BLOCK_SIZE)), seq};

                    // off_t found_block_offset = offset + i * PROCESS_BLOCK_SIZE;
                    off_t internal_offset = static_cast<const char*>(found) - static_cast<const char*>(block);
                    res.internal_offset = internal_offset;

                    // 这里加点输出
                    std::lock_guard<std::mutex> lock(results_mutex);
                    std::cout << "Found UUID at offset: " << res.offset << ", seq: " << res.seq << ", internal_offset: " << res.internal_offset << "\n";
                    results.push_back(res);
                }
            }
        }
    }

    free(buffer);
    io_destroy(ctx);
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " /dev/sdX <uuid>\n";
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

    off_t file_size = get_file_size(disk_path);
    if (file_size <= 0) {
        std::cerr << "Invalid file size.\n";
        return 1;
    }

    off_t segment_size = file_size / NUM_THREADS;
    std::vector<std::thread> threads;
    std::vector<Result> results;
    std::mutex results_mutex;

    for (size_t i = 0; i < NUM_THREADS; ++i) {
        off_t start = i * segment_size;
        off_t end = (i == NUM_THREADS - 1) ? file_size : (i + 1) * segment_size;
        threads.emplace_back(scan_range_libaio, std::ref(disk_path), std::ref(uuid_bytes), start, end, std::ref(results), std::ref(results_mutex));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) { return a.offset < b.offset; });

    for (const auto& r : results) {
        std::cout << "Found UUID at offset: " << r.offset << ", seq: " << r.seq << ", internal_offset: " << r.internal_offset << "\n";
    }

    return 0;
}
