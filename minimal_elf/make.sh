#!/bin/sh

echo "[+] Compile Tiny-Elf Generator"
gcc -o output/tiny_elf_generator minimal_elf_generator.c

echo "[+] Generate Tiny-Elf using Tiny-Elf-Generator"
./output/tiny_elf_generator > ./output/minimal_elf_c

echo "[+] Generate Tiny-Elf using Python-Script"
python3 minimal_elf.py 48b868656c6c6f2e0a005048c7c00100000048c7c7010000004889e648c7c2070000000f0548c7c03c0000004831ff0f05 > output/minimal_elf_py
chmod +x output/minimal_elf_py
