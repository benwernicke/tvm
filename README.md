# tvm

## Instructions

```asm

mov  a b            // a = b
cmov a b c          // if(c) a = b
call a              // ret = ip; ip = a

// arithmetic
add a b c           // a = b + c
sub a b c           // a = b - c
mul a b c           // a = b * c
div a b c           // a = b / c
mod a b c           // a = b % c

// bit manipulation
comp a b            // a = ~b
and  a b c          // a = b & c
or   a b c          // a = b | c
xor  a b c          // a = b ^ c
sr   a b c          // a = b >> c
sl   a b c          // a = b << c

// logic
not a b             // a = !b
eq  a b c           // a = b == c
gt  a b c           // a = b > c
lt  a b c           // a = b < c

// memory
load a b c          // memcpy(&a, b, c)
stor a b c          // memcpy(a, &b, c)

// utility
prnt a              // putc(a, stdout)
aloc a b            // a = malloc(b)
free a              // free(a)
```
