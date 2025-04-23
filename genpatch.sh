#!/bin/bash

# 检查是否提供了起始commit
if [ $# -lt 1 ]; then
    echo "用法: $0 <起始commit>"
    exit 1
fi

START_COMMIT=$1

# 设置源码目录
SOURCE_DIR="/ceph/ceph"

# 检查源码目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "错误: 源码目录 $SOURCE_DIR 不存在"
    exit 1
fi

# 获取当前日期和时间
DATE_STR=$(date "+%m%d")  # 月日，例如0422
TIME_STR=$(date "+%H%M")  # 时分，例如1407

# 设置输出目录和文件名
OUTPUT_DIR="/tmp/ceph_patches"
TAR_NAME="ceph_patches-${DATE_STR}-${TIME_STR}.tar.gz"

# 清理并重建输出目录
echo "清理输出目录..."
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

# 切换到源码目录
cd "$SOURCE_DIR" || { echo "无法进入源码目录"; exit 1; }

# 生成patch文件
echo "生成从 $START_COMMIT 到最新提交的补丁文件..."
git format-patch "$START_COMMIT"..HEAD --output-directory "$OUTPUT_DIR"

# 检查是否成功生成了补丁文件
PATCH_COUNT=$(ls -1 "$OUTPUT_DIR"/*.patch 2>/dev/null | wc -l)
if [ "$PATCH_COUNT" -eq 0 ]; then
    echo "警告: 没有生成任何补丁文件，请检查提供的起始commit是否正确"
    exit 1
fi

echo "成功生成 $PATCH_COUNT 个补丁文件"

# 打包补丁文件
echo "正在创建tar包..."
cd /tmp || { echo "无法进入/tmp目录"; exit 1; }
tar -czvf "$TAR_NAME" "ceph_patches"

# 移动tar包到目标位置
echo "移动tar包到 /ceph/ 目录..."
mv "$TAR_NAME" "/ceph/"

echo "完成! 补丁包已保存为 /ceph/$TAR_NAME"

