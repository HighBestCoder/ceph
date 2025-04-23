#!/bin/bash

# 进入 /tmp 目录
cd /tmp

# 删除旧的 patch 文件
rm -rf /tmp/ceph_patches

# 重命名新的 patch 文件
mv $1 ceph_patches.tar.gz

# 解压 patch 文件
tar -xzvf ceph_patches.tar.gz

# 应用 patch
cd /ceph/ceph
git am /tmp/ceph_patches/*.patch
