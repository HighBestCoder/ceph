#!/bin/bash

# 设备信息
PARTITIONS=256
UUID=$2
DEVICE_PATH=$1
BINARY="./fix_search"
TOTAL_D_SIZE=`fdisk -l ${DEVICE_PATH}  | grep " bytes," | awk '{print $5}'`

TOTAL_SIZE=$(( (TOTAL_D_SIZE / 4096) * 4096))

# 计算分片参数
PART_SIZE=$((TOTAL_SIZE / 4096 / PARTITIONS * 4096))
echo "总大小: $TOTAL_SIZE bytes"
echo "分片数量: $PARTITIONS"
echo "每片大小: $PART_SIZE bytes"

# 创建日志目录
LOG_DIR="/tmp/logs_fast_scan"
mkdir -p "$LOG_DIR"
echo "日志保存目录: $LOG_DIR"

# 并行执行分片处理
for ((i=0; i<PARTITIONS; i++)); do
    # 这里start, end必须要4096对齐
    START=$((i * PART_SIZE))
    END=$(( (i+1)*PART_SIZE))

    # 最后一个分片处理边界问题
    if [ $i -eq $((PARTITIONS-1)) ]; then
        END=$((TOTAL_SIZE))
    fi

    # 生成日志文件名
    LOG_FILE="${LOG_DIR}/fix_search_${i}.log"
    
    # 执行命令并记录日志
    echo "启动分片 $i ($START-$END)..."
    LENGTH=$((END - START))
    $BINARY "$DEVICE_PATH" "$UUID" "$START" "$LENGTH" > "$LOG_FILE" 2>&1 &
done

# 等待所有后台任务完成
echo "等待所有分片处理完成..."
wait

# 合并所有日志文件
echo "合并日志文件..."
FINAL_LOG="p_fix_search_osd_22_$(date +%Y%m%d_%H%M%S).log"
cat "${LOG_DIR}/fix_search_*.log" > "$FINAL_LOG"

echo "最终日志已保存至: $FINAL_LOG"
