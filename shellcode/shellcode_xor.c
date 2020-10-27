inline int write(int, const void *, int) __attribute__((always_inline));

void _start()
{ 
    char string1[] = "uxprNyzayybtnqvrgnxzspsxxlrjozucarpqmE";
    char string2[] = "34155181353888394756237914906292469998";

    for (int i = 0; i < 38; i++)
        string1[i] = string1[i] ^ string2[i];

    write(1, string1, 38);

    // exit(0):
    for (;;) {
        asm("xor rdi, rdi");
        asm("mov rax, 60");
        asm("syscall");
    }
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

