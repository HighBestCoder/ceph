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
    if (needlelen == 0 || haystacklen < needlelen) return nullptr;
    std::string_view hay(static_cast<const char*>(haystack), haystacklen);
    std::string_view nee(static_cast<const char*>(needle), needlelen);
    auto it = std::search(hay.begin(), hay.end(), std::boyer_moore_searcher(nee.begin(), nee.end()));
    if (it != hay.end()) {
        return static_cast<const void*>(&(*it));
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

    void* buffer = nullptr;
    if (posix_memalign(&buffer, 4096, READ_BLOCK_SIZE) != 0) {
        perror("posix_memalign");
        close(fd);
        return 1;
    }

    uint64_t offset = start_offset;
    while (offset < start_offset + total_length) {
        size_t max_read = std::min<uint64_t>(READ_BLOCK_SIZE, start_offset + total_length - offset);
        ssize_t bytes_read = pread(fd, buffer, max_read, offset);
        if (bytes_read < 0) {
            perror("pread");
            break;
        }
        if (bytes_read == 0) {
            break;  // EOF
        }

        size_t blocks = bytes_read / PROCESS_BLOCK_SIZE;
        for (size_t i = 0; i < blocks; ++i) {
            char* block = static_cast<char*>(buffer) + i * PROCESS_BLOCK_SIZE;
            const void* found = fast_memmem(block, PROCESS_BLOCK_SIZE, uuid_bytes.data(), uuid_bytes.size());
            if (found) {
                const char* found_ptr = static_cast<const char*>(found);
                off_t found_block_offset = offset + i * PROCESS_BLOCK_SIZE;
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

        offset += bytes_read;
    }

    free(buffer);
    close(fd);
    return 0;
}
