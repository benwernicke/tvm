printi_uint: # u64 -> u64
        .@buf: char[64] default
        push r01
        push r02
        push r03

        eq  r01 r00 0
        cjmp .zero r01
        lea r01 .@buf
        push ret
        call .helper
        pop ret
        jmp .end

        .zero:
            lea r01 .@buf
            mov r02 0
            stor r01 r02 1
            add  r01 r01 1
            stor r01 0 1
            jmp .end

        .end:
            lea r02 .@buf           # r02 = .@buf

            push ret
            call strlen             #r00 = strlen(.@buf)
            pop ret

            mov r03 r00             # r03 = strlen(.@buf)
            mov r00 1
            mov r01 1

            syscall             # syscall(SYS_WRITE, STDOUT, .@buf, strlen(.@buf))

            pop r03
            pop r02
            pop r01
            return

        .helper: # u64, char* -> char*
            push r02

            



strlen: # r00 is cstr -> uint64_t
    push r01
    push r02

    .loop:
        load r01 r00 1
        add  r00 r00 1
        eq   r01 r01 0
        cjmp .end r01
        add  r02 r02 1
        jmp .loop

.end:
    mov r00 r02
    pop r02
    pop r01
    return

printi_str: # r00 is cstr -> cstr
    push r01
    mov  r01 r00        # cstr in r01

    push ret
    call strlen         # strlen(cstr) in r00
    pop ret

    push r02
    push r03

    mov r02 r01
    mov r03 r00
    mov r00 1
    mov r01 1

    syscall

    mov r00 r02

    pop r03
    pop r02
    pop r01
