/* Generator for a tiny ELF.
 *
 * Compile with: gcc -o generator minimal_elf_generator.c
 * and execute with: ./generator > minimal_elf
 */

#include <stdio.h>
#include <elf.h>
#include <string.h>

void dump(char *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
        printf("%c", data[i]);
}

int main(int argc, char **argv)
{
    Elf64_Ehdr elf_header = {0};
    Elf64_Phdr phdr = {0};

    // Example 'hello.' shellcode:
    char code[] = "\x48\xb8\x68\x65\x6c\x6c\x6f\x2e\x0a\x00\x50\x48\xc7\xc0\x01\x00\x00\x00\x48\xc7\xc7\x01\x00\x00\x00\x48\x89\xe6\x48\xc7\xc2\x07\x00\x00\x00\x0f\x05\x48\xc7\xc0\x3c\x00\x00\x00\x48\x31\xff\x0f\x05";
    size_t code_length = 49;

    // Fill Elf Header:
    elf_header.e_ident[EI_MAG0] = ELFMAG0;
    elf_header.e_ident[EI_MAG1] = ELFMAG1;
    elf_header.e_ident[EI_MAG2] = ELFMAG2;
    elf_header.e_ident[EI_MAG3] = ELFMAG3;
    elf_header.e_ident[EI_CLASS] = ELFCLASS64;
    elf_header.e_ident[EI_DATA] = ELFDATA2LSB;
    elf_header.e_ident[EI_VERSION] = EV_CURRENT;
    elf_header.e_ident[EI_OSABI] = ELFOSABI_NONE; // todo try out linux
    elf_header.e_ident[EI_PAD] = 0;

    elf_header.e_type = ET_EXEC;
    elf_header.e_machine = 0x3e;
    elf_header.e_version = EV_CURRENT;
    elf_header.e_entry = 0x401000 + 64 + 56;
    elf_header.e_phoff = 64;
    elf_header.e_shoff = 0;
    elf_header.e_flags = 0;
    elf_header.e_ehsize = 64;
    elf_header.e_phentsize = 56;
    elf_header.e_phnum = 1;
    elf_header.e_shentsize = 0;
    elf_header.e_shnum = 0;
    elf_header.e_shstrndx = 0;

    // Fill PHDR:
    phdr.p_type = PT_LOAD;
    phdr.p_offset = 0;
    phdr.p_vaddr = 0x401000;
    phdr.p_paddr = 0x401000;
    phdr.p_filesz = code_length;
    phdr.p_memsz = code_length;
    phdr.p_flags = PF_R | PF_X;
    phdr.p_align = 0;

    // Dump everything:
    dump((char *) &elf_header, 64);
    dump((char *) &phdr, 56);
    dump(code, code_length);

    return 0;
}
