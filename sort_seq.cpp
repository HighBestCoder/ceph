#include <algorithm>  // for std::sort
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>  // for exception handling with stoll/stoi
#include <string>
#include <unordered_map>
#include <vector>

// 定义结构体来存储每一行的数据
struct LogEntry {
    long long offset;  // 使用 long long 以防数值过大
    long long seq;     // 使用 long long 以防数值过大
    int internal_offset;

    // (可选) 重载小于操作符，方便排序 (如果不用 lambda)
    // bool operator<(const LogEntry& other) const {
    //     return seq < other.seq;
    // }
};

void print_missing_ranges(const std::unordered_map<uint64_t, std::vector<LogEntry>>& hash) {
    if (hash.empty()) {
        std::cout << "The hash map is empty." << std::endl;
        return;
    }

    // 收集所有键并排序
    std::vector<uint64_t> keys;
    keys.reserve(hash.size());
    for (const auto& pair : hash) {
        keys.push_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());

    std::vector<std::pair<uint64_t, uint64_t>> missing_ranges;

    // 检查初始区间（从1开始）
    uint64_t start_range = 1;
    auto it = keys.begin();
    while (it != keys.end()) {
        uint64_t current_key = *it;

        if (current_key > start_range) {
            // 发现缺失区间
            missing_ranges.emplace_back(start_range, current_key - 1);
        }

        // 更新下一个预期起始点
        start_range = current_key + 1;
        ++it;
    }

    // 输出结果
    for (const auto& range : missing_ranges) {
        std::cout << range.first;
        if (range.first != range.second) {
            std::cout << " " << range.second;
        }
        std::cout << " missing";
        std::cout << std::endl;
    }

    // 然后我们再从keys的最后往前找，我们希望找到最后一个连续的区间
    uint64_t last_key = keys.back();
    std::pair<uint64_t, uint64_t> last_range(last_key, last_key);
    for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
        if (*it == last_key - 1) {
            last_range.first = *it;
            last_key = *it;
        } else {
            break;
        }
    }

    std::cout << "last range: ";
    // 输出最后一个连续区间
    std::cout << last_range.first;
    std::cout << " " << last_range.second;
    std::cout << std::endl;
}

// 函数：用于解析单行文本并返回 LogEntry 结构体
// 如果解析失败，可以抛出异常或返回一个可选类型 (这里选择抛出异常)
LogEntry parseLine(const std::string& line) {
    LogEntry entry;
    size_t offsetPos = line.rfind(" offset: ");
    size_t seqPos = line.rfind(" seq: ");
    size_t internalOffsetPos = line.rfind(" internal_offset: ");

    // 检查是否找到了所有关键字
    if (offsetPos == std::string::npos || seqPos == std::string::npos || internalOffsetPos == std::string::npos) {
        throw std::runtime_error("Malformed line (missing keywords): " + line);
    }

    try {
        // 提取 offset 值
        size_t offsetNumStart = offsetPos + std::string("offset: ").length();
        size_t offsetNumEnd = line.find(',', offsetNumStart);          // 找到数字后的逗号
        if (offsetNumEnd == std::string::npos) offsetNumEnd = seqPos;  // 如果逗号找不到，可能是行尾或其他格式
        entry.offset = std::stoull(line.substr(offsetNumStart, offsetNumEnd - offsetNumStart));

        // 提取 seq 值
        size_t seqNumStart = seqPos + std::string("seq: ").length();
        size_t seqNumEnd = line.find(',', seqNumStart);                     // 找到数字后的逗号
        if (seqNumEnd == std::string::npos) seqNumEnd = internalOffsetPos;  // 如果逗号找不到
        entry.seq = std::stoull(line.substr(seqNumStart, seqNumEnd - seqNumStart));

        // 提取 internal_offset 值
        size_t internalOffsetNumStart = internalOffsetPos + std::string("internal_offset: ").length();
        // 这个值是行末尾，所以不需要找逗号
        entry.internal_offset = std::stoi(line.substr(internalOffsetNumStart));

    } catch (const std::invalid_argument& e) {
        throw std::runtime_error("Invalid number format on line: " + line + " (" + e.what() + ")");
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Number out of range on line: " + line + " (" + e.what() + ")");
    }

    return entry;
}

void write_hashmap_to_file(std::vector<LogEntry>& logEntries) {
    const char* filename = "/tmp/bluefs_seq_offset_map";

    // 打开文件
    FILE* file = fopen(filename, "w");
    if (file == nullptr) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // 首先拿到所有的log_seq的keys

    for (auto& p : logEntries) {
        auto log_seq = p.seq;
        auto offset = p.offset;
        auto internal_offset = p.internal_offset;

        // 写这两个数据对
        fprintf(file, "%llu %llu\n", log_seq, offset);
    }

    // 关闭文件
    if (fclose(file) != 0) {
        std::cerr << "Error closing file: " << filename << std::endl;
    } else {
        std::cout << "Data written to " << filename << " successfully." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // 1. 检查命令行参数
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;  // 返回错误码
    }

    std::string filename = argv[1];
    std::ifstream inputFile(filename);

    // 2. 检查文件是否成功打开
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;  // 返回错误码
    }

    std::vector<LogEntry> logEntries;
    std::string currentLine;
    int lineNumber = 0;

    // 3. 逐行读取和解析文件
    while (std::getline(inputFile, currentLine)) {
        lineNumber++;
        if (currentLine.empty()) {  // 跳过空行
            continue;
        }
        try {
            LogEntry entry = parseLine(currentLine);
            logEntries.push_back(entry);
        } catch (const std::runtime_error& e) {
            // 报告解析错误，但继续处理文件的其余部分
            std::cerr << "Warning: Skipping line " << lineNumber << " due to error: " << e.what() << std::endl;
        }
    }

    inputFile.close();  // 关闭文件

    // 4. 按照 seq 排序
    std::sort(logEntries.begin(), logEntries.end(), [](const LogEntry& a, const LogEntry& b) {
        if (a.seq != b.seq) return a.seq < b.seq;  // 升序排序
        return a.offset < b.offset;
    });

    std::unordered_map<uint64_t /*log_seq*/, std::vector<LogEntry>> hash;
    for (const auto& entry : logEntries) {
        hash[entry.seq].push_back(entry);
    }

    // 输出second.size() > 1的Item
    for (const auto& [key, value] : hash) {
        if (value.size() > 1) {
            std::cout << "seq: " << key << " has " << value.size() << " entries:" << std::endl;
        }
    }

    print_missing_ranges(hash);

    // 写入文件
    write_hashmap_to_file(logEntries);

    return 0;  // 程序成功结束
}
