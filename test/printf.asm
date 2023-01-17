printf: # char* fmt string ...
    .@buf: char[1024] default
    # r00 fmt_string
    # r01 buf_ptr
    # r02 buf_index
    # r03 current char
    # r04 for equality

    push r01
    push r02
    push r03
    push r04

    lea r01 .@buf
    mov r02 0
    mov r03 0

    .loop:
        load r03 r00 1      # load current char
        add  r00 r00 1      # increment fmt_str ptr
        eq   r04 r03 0      # test if end of string
        cjmp .end r04       # end loop if so
        stor r01 r03 1      # else stor char to buf
        add  r01 r01 1      # increment buf ptr
        add  r02 r02 1      # increment buf len
        jmp  .loop

.end:
    mov r03 r02
    lea r02 .@buf       
    mov r00 1           
    mov r01 1           
    syscall                 # SYSCALL( WRITE, STDOUT, .@buf, buf_len )
    
    pop r04
    pop r03
    pop r02
    pop r01
    return

main:
    .@fmt_str: char[100] "Hello World!\n\0"
    lea r00 .@fmt_str
    call printf
    exit 0
