set -xe
make -j4
riscv64-elf-gcc -Wall -nostdlib -mno-relax -static \
	-nostartfiles -march=rv32izicsr -mabi=ilp32 -o test/out.bin test/boot.c -T test/kernel.ld
./build/sim test/out.bin
