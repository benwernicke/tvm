jmp main


print_str: # ptr len
    push r02
    push r03

    mov r02 r00
    mov r03 r01
    mov r00 1
    mov r01 1

    syscall

    pop r03
    pop r02
    return

main:
    .@string: char[14] "Hello World!\n"
    lea r00 .@string
    mov r01 13
    call print_str
    exit 0
