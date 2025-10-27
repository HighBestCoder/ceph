#!/bin/bash
# pack-ceph-tools.sh

# 创建打包目录
PACK_DIR="ceph-tools-package-osd35"
mkdir -p $PACK_DIR/{bin,lib}

# 复制主程序
cp ../build/bin/ceph-bluestore-tool $PACK_DIR/bin/
cp ../build/bin/ceph-objectstore-tool $PACK_DIR/bin/

# 复制自定义库(非系统库)
cp -rfL /lib64/libfmt.so.6.2.1 $PACK_DIR/lib/
cp /home/build/lib/libceph-common.so.2 $PACK_DIR/lib/

# 创建启动脚本
cat > $PACK_DIR/ceph-bluestore-tool.sh <<'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
exec "$SCRIPT_DIR/bin/ceph-bluestore-tool" "$@"
EOF

cat > $PACK_DIR/ceph-objectstore-tool.sh <<'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
exec "$SCRIPT_DIR/bin/ceph-objectstore-tool" "$@"
EOF

chmod +x $PACK_DIR/ceph-bluestore-tool.sh
chmod +x $PACK_DIR/ceph-objectstore-tool.sh

# 创建使用说明
cat > $PACK_DIR/README.txt <<'EOF'
Ceph 工具包使用说明

此包包含以下工具：
- ceph-bluestore-tool: BlueStore 管理工具
- ceph-objectstore-tool: ObjectStore 管理工具

使用方法：
./ceph-bluestore-tool.sh [参数]
./ceph-objectstore-tool.sh [参数]

依赖库已包含在 lib 目录中，无需额外安装。
EOF

# 打包
tar czf ceph-tools-package-osd35.tar.gz $PACK_DIR/

echo "打包完成: ceph-tools-package-osd35.tar.gz"
echo "包含工具: ceph-bluestore-tool, ceph-objectstore-tool"
