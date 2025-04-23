#!/bin/bash

# 编译 search.cpp (需要 -laio)
g++ -std=c++17 /ceph/ceph/search.cpp -o /ceph/ceph/build/bin/search -laio

# 编译 fix_search.cpp
g++ -std=c++17 /ceph/ceph/fix_search.cpp -o /ceph/ceph/build/bin/fix_search

# 编译 sort_seq.cpp
g++ -std=c++17 /ceph/ceph/sort_seq.cpp -o /ceph/ceph/build/bin/sort_seq

echo "编译完成"
