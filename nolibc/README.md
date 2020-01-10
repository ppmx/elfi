# nolibc experiment

This code base provides a framework to create a binary in order to perform responsive binary analysis, and it provides the opportunity to create binary-independent (shell-) code, because it is compiled and linked as a very spare ELF binary.


## Insights

GCC provides a set of [builtin-functions](https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html). In order to avoid warnings due to non-matching function signatures (that are not defined in the source code, so they come from nowhere (the compiler)) and in order to avoid changes due to optimization, these builtin functions are disabled.