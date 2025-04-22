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

// 函数：用于解析单行文本并返回 LogEntry 结构体
// 如果解析失败，可以抛出异常或返回一个可选类型 (这里选择抛出异常)
LogEntry parseLine(const std::string& line) {
    LogEntry entry;
    size_t offsetPos = line.rfind("offset: ");
    size_t seqPos = line.rfind("seq: ");
    size_t internalOffsetPos = line.rfind("internal_offset: ");

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

    std::unordered_map<uint64_t, std::vector<LogEntry>> hash;
    for (const auto& entry : logEntries) {
        hash[entry.seq].push_back(entry);
    }

    // 输出second.size() > 1的Item
    for (const auto& [key, value] : hash) {
        if (value.size() > 1) {
            std::cout << "seq: " << key << " has " << value.size() << " entries:" << std::endl;
            for (const auto& entry : value) {
                std::cout << "  offset: " << entry.offset << ", internal_offset: " << entry.internal_offset << std::endl;
            }
        }
    }

    // 我们还需要找到缺失的seq
    std::vector<uint64_t> seqs;

    // 首先找最大的seq
    uint64_t max_seq = 0;
    for (const auto& [key, value] : hash) {
        if (key > max_seq) {
            max_seq = key;
        }
    }

    // 检查有没有缺失的seq
    // 这里假设seq是从1开始的连续整数
    std::cout << "max_seq: " << max_seq << std::endl;
    // 然后找出所有的seq
    for (uint64_t i = 1; i <= max_seq; ++i) {
        if (hash.find(i) == hash.end()) {
            seqs.push_back(i);
        }
    }

    // 输出缺失的seq
    if (!seqs.empty()) {
        std::cout << "Missing seqs: ";
        for (const auto& seq : seqs) {
            std::cout << seq << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "No missing seqs." << std::endl;
    }

    return 0;  // 程序成功结束
}
