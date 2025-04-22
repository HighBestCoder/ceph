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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " /dev/sdX <uuid>\n";
        return 1;
    }

    std::string disk_path = argv[1];
    std::string uuid_str;
    try {
        uuid_str = parse_uuid_to_bytes(argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing UUID: " << e.what() << "\n";
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

    size_t block_index = 0;
    while (true) {
        off_t offset = block_index * READ_BLOCK_SIZE;
        ssize_t bytes_read = pread(fd, buffer, READ_BLOCK_SIZE, offset);
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
            const void* found = fast_memmem(block, PROCESS_BLOCK_SIZE, uuid_str.data(), uuid_str.size());
            if (found) {
                off_t found_block_offset = offset + i * PROCESS_BLOCK_SIZE;
                off_t internal_offset = static_cast<const char*>(found) - static_cast<const char*>(block);
                std::cout << "Found UUID in block starting at offset: " << found_block_offset << " internal_offset:" << internal_offset << "\n";
            }
        }
        ++block_index;
    }

    free(buffer);
    close(fd);
    return 0;
}
