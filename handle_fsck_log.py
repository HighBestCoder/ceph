#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BlueFS日志解析工具 v1.1

功能说明：
    从BlueFS日志中提取jump_seq和offset字段，生成简化格式输出

作者：AI助手
创建日期：2025-04-26
版本：1.1.0
"""

import re
import sys
from typing import Optional, Tuple

def parse_log(log_line: str) -> Optional[Tuple[int, int]]:
    """
    解析单行BlueFS日志，提取jump_seq和offset数值
    
    Args:
        log_line (str): 待解析的日志行
    
    Returns:
        Optional[Tuple[int, int]]: 包含(offset, jump_seq)的元组，解析失败返回None
    
    示例：
        >>> parse_log("jump_seq = 509691603, offset = 721053220864")
        (721053220864, 509691603)
    """
    # 正则表达式匹配模式（含命名分组）
    # 同时验证关键字段存在性
    if not (re.search(r'MAP', log_line) and 
            re.search(r'jump_seq\s*=', log_line) and 
            re.search(r'offset\s*=', log_line)):
        return None
    pattern = r'jump_seq\s*=\s*(?P<jump_seq>\d+),\s*offset\s*=\s*(?P<offset>\d+)'
    match = re.search(pattern, log_line)
    
    if match:
        try:
            # 转换为整数并返回
            return int(match.group('offset')), int(match.group('jump_seq'))
        except ValueError as e:
            # 记录数值转换错误（实际应用建议使用logging）
            print(f"Invalid numeric value: {log_line}", file=sys.stderr)
            return None
    return None

def process_log_stream(stream) -> None:
    """
    处理日志流并输出解析结果
    
    Args:
        stream: 输入流对象（支持文件对象、sys.stdin等）
    """
    for line in stream:
        stripped = line.strip()
        # 跳过空行和注释行
        if not stripped or stripped.startswith('#'):
            continue
        
        result = parse_log(stripped)
        if result:
            offset, jump_seq = result
            print(f"{offset}\t{jump_seq}")

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