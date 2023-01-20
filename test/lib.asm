:@buf:     char[1024] default
:@buf_len: u64        default

print_cstr: # s: char* -> char*
    push r01
    push r02
    push r03
    push r04

    mov r01 r00     # safe s for later
    mov r04 r01

    push ret
    call strlen     # get strlen(s)
    call :check_flush
                    # r00: sz, r01: s and buf has enough space

    mov r02 r00     # r02 = sz

    # load buffer with offset
    lea r00 :@buf
    lea r03 :@buf_len
    load r03 r03 8
    add r00 r00 r03

    call memcpy

    add r03 r03 r02
    lea r00 :@buf_len
    stor r00 r03 8

    mov r00 r04     # for return

    pop ret
    pop r04
    pop r03
    pop r02
    pop r01
    return



memcpy: # d: void*, s: void*, sz: u64 -> none
        push r03     # num currently pushed
        push r04     # intermediate value
        push r05     # for equailty
        mov r03 0

        .loop:
            geq r05 r03 r02  # r05 = r03 >= r02
            cjmp .end r05    # end loop if so
            load r04 r01 1  
            stor r00 r04 1   # copy on byte from r01 to r00
            inc r00
            inc r01
            inc r03          # increcment all pointer
            jmp .loop

    .end:
        pop r05
        pop r04
        pop r03
        return

flush: # none -> none
    push r00
    push r01
    push r02
    push r03

    mov r00 1
    mov r01 1
    lea r02 :@buf
    lea r03 :@buf_len
    load r03 r03 8

    syscall

    lea r03 :@buf_len
    stor r03 0 8

    pop r03
    pop r02
    pop r01
    pop r00
    return

:check_flush: # sz: u64 -> u64
        push r01
        
        lea r01 :@buf_len
        load r01 r01 8
        add r01 r01 r00
        geq r01 r01 1024
        cjmp .call_flush r01

    .end:
        pop r01
        return

    .call_flush:
        push ret
        call flush
        pop ret
        jmp .end

strlen: # s: char* -> u64
        push r01
        push r02

        .loop:
            load r01 r00 1      # r01 = *s
            inc  r00            # s++
            eq   r01 r01 0      # r01 = r01 == 0;
            cjmp .end r01
            inc  r02 
            jmp .loop
    .end:
        mov r00 r02 
        pop r02
        pop r01
        return
