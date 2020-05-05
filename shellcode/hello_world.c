#define STRLEN(s) (sizeof(s)/sizeof(s[0]))

inline int write(int, const void *, int) __attribute__((always_inline));

int write(int fildes, const void *buf, int nbyte)
{
	int ret;

	asm("mov rdi, %0" : : "r"((long) fildes));
	asm("mov rsi, %0" : : "r"(buf));
	asm("mov rdx, %0" : : "r"((long) nbyte));

	asm("mov rax, 1");
	asm("syscall");

	asm("mov %0, eax" : "=r" (ret));

	return ret;
}

void _start()
{
	char s[] = "Hello world.\n";

	write(1, s, STRLEN(s));

	// exit(0):
	for (;;) {
		asm("xor rdi, rdi");
		asm("mov rax, 60");
		asm("syscall");
	}
}
