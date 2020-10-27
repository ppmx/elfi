src = $(wildcard *.c)
obj = $(src:.c=.o)
scs = $(src:.c=.sc)

all: build $(obj) $(scs)

%.o: %.c
	gcc -fno-stack-protector -masm=intel -nostdlib -fno-builtin -Wall -Wextra -o build/$@ $<

%.sc: %.o
	objcopy -O binary --only-section=.text build/$< build/$@

build:
	mkdir -p build

clean:
	rm -rf build

