void exit(int status)
{
    register int reg_status asm("rdi") = status;

    asm("mov rax, 60");
    asm("syscall");
}

int puts(const char *s)
{
    // preparation of arguments:
    asm("mov rdi, 1");
    register const char *reg_s asm("rsi") = s;
    asm("mov rdx, 12");

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