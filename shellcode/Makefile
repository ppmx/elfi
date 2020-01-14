all:
	gcc -fno-stack-protector -masm=intel -nostdlib -fno-builtin -Wall -Wextra -o output/hello_world hello_world.c
	gcc -fno-stack-protector -masm=intel -nostdlib -fno-builtin -Wall -Wextra -o output/execve execve.c

clean:
	rm -rf output/*
