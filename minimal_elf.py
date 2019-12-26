#!/usr/bin/env python3

""" ELF - Executable and Linking Format

ELF is a common standard file format for executable files. This is a generator
for probable the smallest ELF that is possible.


Example:
========
$ python3 minimal_elf.py 48b868656c6c6f2e0a005048c7c00100000048c7c7010000004889e648c7c2070000000f0548c7c03c0000004831ff0f05 > minimal_elf
$ chmod +x minimal_elf
$ ./minimal_elf
hello.
"""

import struct
import sys

def generate_minimal_elf(code):
    """ This function generates the content of the resulting ELF file
    containing the given code (bytes) as entry.

    The minimal structure of an ELF consists of the ELF header (fixed size) and
    one executable segment with its program header table entry.

    +------------------------+
    |       ELF Header       |
    +------------------------+
    |  Program Header Table  |
    |      (one entry)       |
    +------------------------+
    | Segment 1: containing  |
    |    the given shellcode |
    +------------------------+

    See also linux/include/uapi/linux/elf.h
    """

    # all in all it is 64 bytes long
    elf_header = [
        # building e_ident
        b'\x7fELF',  # magic number
        b'\x02',     # EI_CLASS (file class): ELFCLASS64
        b'\x01',     # EI_DATA (data encoding): ELFDATA2LSB
        b'\x01',     # EI_VERSION (file version) = EV_CURRENT defined in e_version
        b'\x00',     # EI_OSABI (0 regardless of the target platform)
        b'\x00' * 8, # ABIVERSION and EI_PAD (start of padding bytes) = reserved and set to zero
        b'',         # EI_NIDENT (size of e_ident[])

        # e_type
        struct.pack('<H', 2), # executable file

        # e_machine
        struct.pack('<H', 0x3e), # x86-64 architecture

        # e_version
        struct.pack('<I', 1),

        # e_entry = start of segment in virtual address
        # = virtual address + offset of segment from beginning of ELF file
        struct.pack('<Q', 0x401000 + 64 + 56),

        # e_phoff = sizeof elf_header
        struct.pack('<Q', 64),

        # e_shoff = no section header
        struct.pack('<Q', 0),

        # e_flags
        struct.pack('<I', 0),

        # e_ehsize - size of elf header in bytes
        struct.pack('<H', 64),

        # e_phentsize - size of a program header table entry
        struct.pack('<H', 56),

        # e_phnum - number of entries in the program header table
        struct.pack('<H', 1),

        # e_shentsize - size of a section header table entry
        struct.pack('<H', 0),

        # e_shnum - number of entries in the section header table
        struct.pack('<H', 0),

        # e_shstrndx
        struct.pack('<H', 0)
    ]

    assert (len(b''.join(elf_header)) == 64)

    # one entry in the program header, in sum 56 bytes long:
    program_header_table = [
        # p_type = PT_LOAD
        struct.pack('<I', 1),

        # p_flags = PF_X | PF_R
        struct.pack('<I', 0x1 | 0x4),

        # p_offset = map the whole ELF to memory
        struct.pack('<Q', 0),

        # p_vaddr and p_paddr
        struct.pack('<Q', 0x401000),
        struct.pack('<Q', 0x401000),

        struct.pack('<Q', 64 + 56 + len(code)),  # p_filesz
        struct.pack('<Q', 64 + 56 + len(code)),  # p_memsz

        # p_align
        struct.pack('<Q', 0)
    ]

    assert (len(b''.join(program_header_table)) == 56)

    return b''.join(elf_header) + b''.join(program_header_table) + code

def main():
    """ Write minimal ELF to STDOUT """

    try:
        shellcode = bytes.fromhex(sys.argv[1])
    except IndexError:
        print("Usage:", sys.argv[0], "<Shellcode - hex encoded>")
        return

    elf = generate_minimal_elf(shellcode)
    sys.stdout.buffer.write(elf)

if __name__ == "__main__":
    main()
