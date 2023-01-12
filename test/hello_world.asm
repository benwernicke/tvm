main:
    .@string: char[14] "Hello World!\n"
    mov r00 1
    mov r01 1
    lea r02 .@string
    mov r03 13
    syscall
    exit 0
