#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
UUID日志解析工具 v1.1

功能说明：
    从指定格式的日志中提取offset和seq字段，生成对齐的制表符分隔输出
    严格验证日志行格式：Found UUID at offset: %d, seq: %d, internal_offset: %d

作者：AI助手
创建日期：2025-04-26
版本：1.1.0
"""

import re
import sys
from typing import Optional, Tuple

# 预编译正则表达式提升性能
PATTERN = re.compile(
    r'^Found UUID at offset:\s*(?P<offset>\d+),\s*seq:\s*(?P<seq>\d+),\s*internal_offset:\s*(?P<internal_offset>\d+)$'
)

def parse_log_line(line: str) -> Optional[Tuple[int, int]]:
    """解析单行日志，提取offset和seq数值
    
    Args:
        line (str): 待解析的日志行
        
    Returns:
        Optional[Tuple[int, int]]: 包含(offset, seq)的元组，解析失败返回None
        
    示例：
        >>> parse_log_line("Found UUID at offset: 3154926190592, seq: 455786603, internal_offset: 6")
        (3154926190592, 455786603)
    """
    match = PATTERN.match(line)
    if not match:
        print(f"Invalid log format: {line}", file=sys.stderr)
        return None
    
    try:
        return int(match.group('offset')), int(match.group('seq'))
    except ValueError as e:
        print(f"Numeric conversion error: {line}", file=sys.stderr)
        return None

def process_log_stream(stream) -> None:
    """处理日志流并输出解析结果
    
    Args:
        stream: 输入流对象（支持文件对象、sys.stdin等）
    """
    for line in stream:
        stripped = line.strip()
        # 跳过空行和注释行
        if not stripped or stripped.startswith('#'):
            continue
        
        result = parse_log_line(stripped)
        if result:
            offset, seq = result
            print(f"{offset}\t{seq}")

if __name__ == "__main__":
    # 命令行参数处理
    if len(sys.argv) > 1:
        try:
            with open(sys.argv[1], 'r') as f:
                process_log_stream(f)
        except FileNotFoundError:
            print(f"Error: File not found - {sys.argv[1]}", file=sys.stderr)
            sys.exit(1)
    else:
        process_log_stream(sys.stdin)