#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
序列连续性检测工具 v1.1

功能说明：
    检测fix_search_map中的seq连续性，并在header_map中查找可用版本
    支持百万级数据的高效处理

作者：AI助手
创建日期：2025-04-26
版本：1.1.0
"""

import sys
from collections import OrderedDict
from typing import Dict, List, Optional, Tuple

def read_seq_file(file_path: str) -> Dict[int, int]:
    """读取seq文件并构建字典
    
    Args:
        file_path: 文件路径
        
    Returns:
        Dict[int, int]: {seq: disk_offset}字典
        
    示例：
        >>> read_seq_file("fix_search_map")
        {1: 100, 2: 200, 3: 300}
    """
    seq_dict = OrderedDict()
    try:
        with open(file_path, 'r') as f:
            for line in f:
                stripped = line.strip()
                if not stripped:
                    continue
                parts = stripped.split()
                if len(parts) != 4 or parts[0] != 'Found':
                    continue
                try:
                    seq = int(parts[2].split(':')[1])
                    offset = int(parts[3].split(':')[1])
                    seq_dict[seq] = offset
                except (IndexError, ValueError):
                    continue
    except FileNotFoundError:
        print(f"Error: File not found - {file_path}", file=sys.stderr)
        sys.exit(1)
    return seq_dict

def find_continuous_sequence(fix_dict: Dict[int, int], header_dict: Dict[int, int]) -> Optional[Tuple[int, List[int]]]:
    """查找连续序列并返回可用版本
    
    Args:
        fix_dict: fix_search_map字典
        header_dict: header_map字典
        
    Returns:
        Optional[Tuple[int, List[int]]]: (起始seq, disk_offsets列表)或None
    """
    if not fix_dict:
        print("Error: fix_search_map is empty", file=sys.stderr)
        return None
    
    max_seq = max(fix_dict.keys())
    sorted_seqs = sorted(fix_dict.keys(), reverse=True)
    
    break_point = None
    for i in range(1, len(sorted_seqs)):
        if sorted_seqs[i-1] - sorted_seqs[i] > 1:
            break_point = sorted_seqs[i]
            break
    
    if break_point is None:
        # 全部连续的情况
        return (max_seq, [fix_dict[max_seq]])
    
    candidate_seq = break_point + 1
    if candidate_seq in header_dict:
        # 找到可用版本
        start_seq = candidate_seq
        end_seq = max_seq
        return (start_seq, [fix_dict[s] for s in range(start_seq, end_seq+1)])
    
    # 未找到可用版本，调整max_seq
    new_max = break_point
    if new_max in fix_dict:
        return find_continuous_sequence({k:v for k,v in fix_dict.items() if k >= new_max}, header_dict)
    return None

def main():
    if len(sys.argv) != 3:
        print("Usage: python3 seq_checker.py fix_search_map header_map", file=sys.stderr)
        sys.exit(1)
    
    fix_dict = read_seq_file(sys.argv[1])
    header_dict = read_seq_file(sys.argv[2])
    
    result = find_continuous_sequence(fix_dict, header_dict)
    
    if result:
        start_seq, offsets = result
        print(f"Found continuous sequence starting at seq {start_seq}:")
        for offset in offsets:
            print(offset)
    else:
        print("No valid sequence found", file=sys.stderr)

if __name__ == "__main__":
    main()