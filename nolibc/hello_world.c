void exit (int) __attribute__ ((noreturn));
int puts(const char *);
void _start () __attribute__ ((noreturn));
int write(int, const void *, int);

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
    int strlen = 0;

    // Compute length of s and store length in rdx:
    for (char *ptr = (char *) s; *ptr != '\0'; ptr++)
        strlen += 1;

    return write(1, s, strlen);
}

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
    exit(puts("hello world.\n"));
}