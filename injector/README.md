# Segment-Padding-Injector

Infects a given ELF-binary by storing some shellcode in a pad between two segments, while adjusting the entry point to start at the shellcode and jump to the original code after executing this.

## Example

As example we take a simple "hello world". We infect it with a shellcode that forks and spawns a browser in one process, continuing in the other process:

```
$ ./example/hello_world
Hello World!

$ ./build/infect ./example/hello_world $(cat  example/shellcode)
...

$ ./example/hello_world
Hello Wold! [browser opens in the background]
```

