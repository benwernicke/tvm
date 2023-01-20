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

print_uint:
    .@buf:  char[64] default

    push r01
    push r02

    lea r02 .@buf
    add r02 r02 62

    .L1:
        mod r01 r00 10
        add r01 r01 '0'     # extract last digit
        
        stor r02 r01 1
        sub  r02 r02 1      # stor and decr buf_ptr

        div r00 r00 10
        neq r01 r00 0
        cjmp .L1 r01

    add r00 r02 1       # ptr is in r00 for print_str call
    lea r02 .@buf
    add r02 r02 63
    sub r01 r02 r00

    push ret
    call print_str
    pop ret

    pop r02
    pop r01
    return

main:
    mov r00 69420
    call print_uint
    prnt '\n'
    exit 0
