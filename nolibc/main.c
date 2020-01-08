void exit(int status)
{
    register int reg_status asm("rdi") = status;

    asm("mov rax, 60");
    asm("syscall");
}

int _strlen(char *s)
{
    int slen = 0;

    while (*s++ != '\0')
        slen += 1;

    return slen;
}

int puts(const char *s)
{
    // preparation of arguments:
    //register int strlen asm("rdx") = _strlen(s);

    asm("xor rdx, rdx");

    for (char *ptr = s; *ptr != '\0'; ptr++)
        asm("inc rdx");

    register const char *reg_s asm("rsi") = s;
    asm("mov rdi, 1");

    // 'write' syscall:
    asm("mov rax, 1");
    asm("syscall");

    // puts must return a non-negative number on success:
    return 0;
}

void _start()
{
    puts("hello world\n");
    exit(42);
}