#!/bin/bash

# Run QEMU with GDB server for assembly-only example
echo "Starting QEMU with GDB server on port 1234..."
echo "In another terminal, run: gdb-multiarch encrypt.elf/decrypt.elf"
echo "Then in GDB: target remote :1234"

qemu-system-riscv32 \
    -machine virt \
    -nographic \
    -bios none \
    -kernel encrypt.elf \
    -S \
    -gdb tcp::1234