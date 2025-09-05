#!/usr/bin/bash
# Build script for assembly-only example
echo "Building tea encrypt/decrypt..."

riscv64-unknown-elf-gcc \
    -march=rv32im \
    -mabi=ilp32 \
    -nostdlib \
    -ffreestanding \
    -g \
    decrypt.s \
    -T linker.ld \
    -o decrypt.elf

if [ $? -eq 0 ]; then
    echo "Build successful: decrypt.elf created"
else
    echo "Build failed"
    exit 1
fi