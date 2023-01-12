jmp main

print_number:
    push  r01
    neq   r01 r00 0         
    cjmp  .L1 r01               # if r00 == 0
        prnt 48
        jmp  .end
    .L1:                        # else
        push ret
        call .helper
        pop  ret
        jmp  .end

.end:
    pop r01
    return

.helper:
    push r01
    eq   r01 r00 0
    cjmp .helper.end r01            # if r00 == 0 goto helper.end
    mod r01 r00 10
    add r01 r01 48
    div r00 r00 10
    push ret
    call .helper
    pop ret
    prnt r01
    jmp .helper.end

.helper.end:
    pop r01
    return

main:
    mov r00 69
    call print_number
    prnt 10
    exit 0
