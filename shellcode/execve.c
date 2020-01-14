inline int execve(const char *path, char *const argv[], char *const envp[]) __attribute__((always_inline));

int execve(const char *path, char *const argv[], char *const envp[])
{
    int ret;

    asm("mov rdi, %0" : : "r"(path));
    asm("mov rsi, %0" : : "r"(argv));
    asm("mov rdx, %0" : : "r"(envp));

    asm("mov rax, 59");
    asm("syscall");

    asm("mov %0, eax" : "=r" (ret));

    return ret;
}


#define NULL (void *) 0

void _start()
{
    char *args[] = {"/bin/sh", NULL};

    execve(args[0], args, NULL);

    // exit(0):
    for (;;) {
        asm("xor rdi, rdi");
        asm("mov rax, 60");
        asm("syscall");
    }
}