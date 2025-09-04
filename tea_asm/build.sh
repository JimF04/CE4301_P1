#!/usr/bin/bash
# Build script for assembly-only example
echo "Building tea encrypt/decrypt..."

riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g \
    encrypt.s \
    -T linker.ld \
    -o encrypt.elf

if [ $? -eq 0 ]; then
    echo "Build successful: encrypt.elf created"
else
    echo "Build failed"
    exit 1
fi