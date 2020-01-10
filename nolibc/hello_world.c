void exit (int) __attribute__ ((noreturn));
int puts(const char *);
void _start () __attribute__ ((noreturn));


void exit(int status)
{
    asm("mov rdi, %0" : : "r"((long) status));

    // syscall exit:
    asm("mov rax, 60");
    asm("syscall");

    for (;;) exit(status);
}

int puts(const char *s)
{
    // Syscall paramteres:
    asm("mov rdi, 1");
    asm("mov rsi, %0" : : "r"(s));
    asm("xor rdx, rdx");

    // Compute length of s and store length in rdx:
    for (char *ptr = (char *) s; *ptr != '\0'; ptr++)
        asm("inc rdx");

    // syscall write:
    asm("mov rax, 1");
    asm("syscall");

    // puts must return a non-negative number on success:
    return 0;
}

void _start()
{
    puts("hello world.\n");
    exit(23);
}