#!/bin/bash

ceph osd pool create test-pool 1024 1024

# 指定你的Ceph集群池的名称
POOL_NAME="test-pool"

cd /ceph/ceph/src

# 遍历当前目录下的所有文件
for file in `find . -name "*"`
do
    # 使用rados命令将文件放入Ceph集群池中
    rados -p $POOL_NAME put $file $file
done
