#include <stdint.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// 读取序列文件并构建有序字典
std::vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>> read_seq_file(const std::string& file_path) {
    std::vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>> seq_list;
    ifstream file(file_path);

    if (!file.is_open()) {
        throw runtime_error("Error: File not found - " + file_path);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string word;
        vector<string> parts;

        // 分割单词
        while (iss >> word) {
            parts.push_back(word);
        }

        if (parts.empty() || parts.size() != 2) {
            continue;
        }

        // 第一个是disk_offset
        uint64_t disk_offset = std::stoull(parts[0].c_str());

        // 第二个是seq
        uint64_t seq = std::stoull(parts[1].c_str());

        seq_list.push_back({disk_offset, seq});
    }

    // 按照disk_offset排序
    std::sort(seq_list.begin(), seq_list.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    return seq_list;
}

// 查找连续序列并返回可用版本
pair<uint64_t /*disk_offset_header*/, vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>> /*next_seq*/> find_continuous_sequence(
    const std::vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>>& fix_list, const std::vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>>& header_list) {
    if (fix_list.empty()) {
        throw runtime_error("Error: fix_search_map is empty");
    }

    pair<uint64_t /*disk_offset_header*/, vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>> /*next_seq*/> ans;

    // 获取最大序列号
    uint64_t max_seq = fix_list.back().second;

    std::cout << "max_seq: " << max_seq << std::endl;

    ans.first = UINT64_MAX;

    // 查找断点
    bool has_find = false;
    uint64_t break_point = max_seq + 1;
    for (size_t i = fix_list.size() - 1; i > 0; i--) {
        // std::cout << "check seq: " << fix_list[i].second << "max_seq: " << max_seq << " break: " << break_point << std::endl;
        if (fix_list[i].second != break_point - 1) {
            break_point = break_point;  // point to self. it's jump_seq
            has_find = true;
            break;
        }
        break_point = fix_list[i].second;
    }

    // 处理完全连续的情况
    if (!has_find) {
        std::cout << "not found break point" << std::endl;
        return ans;
    } else {
        std::cout << "break_point: " << break_point << std::endl;
    }

    // 检查候选序列
    for (auto& p : header_list) {
        if (p.second == break_point) {
            ans.first = p.first;
            std::cout << "find header jump_seq: " << p.second << std::endl;
            std::cout << "find header disk_offset: " << p.first << std::endl;
            break;
        }
    }

    for (size_t i = fix_list.size() - 1; i > 0; i--) {
        if (fix_list[i].second >= break_point) {
            ans.second.push_back(fix_list[i]);
        }
    }

    // 对ans.second按照seq来排序
    std::sort(ans.second.begin(), ans.second.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

    return ans;
}

void write_offset_seq(const std::string& file_path, const std::vector<std::pair<uint64_t /*disk_offset*/, uint64_t /*seq*/>>& seq_list) {
    ofstream file(file_path);
    if (!file.is_open()) {
        throw runtime_error("Error: Unable to open file for writing - " + file_path);
    }

    for (const auto& p : seq_list) {
        file << p.first << " " << p.second << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " fix_search_map header_map" << endl;
        return 1;
    }

    try {
        // 读取输入文件
        auto fix_dict = read_seq_file(argv[1]);
        write_offset_seq("fix_search_osd_4_map_sort", fix_dict);
        auto header_dict = read_seq_file(argv[2]);

        // 输出调试信息
        cout << "fix_dict size: " << fix_dict.size() << endl;

        // 查找连续序列
        auto ans = find_continuous_sequence(fix_dict, header_dict);

        // 输出结果
        std::cout << "first jump offet: " << ans.first << std::endl;
        std::cout << "next seq: " << std::endl;
        for (const auto& p : ans.second) {
            std::cout << p.first << " " << p.second << std::endl;
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}