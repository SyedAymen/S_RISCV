#/usr/bin/bash

verilator -Wall --cc --trace --exe sim/$1_tb.cpp --top-module $1 -Isrc $(ls src/*.v)
make -C obj_dir -j12 -f V$1.mk V$1
./obj_dir/V$1
