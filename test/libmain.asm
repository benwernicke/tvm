main:
        .@str: char[32] "Hello World!\n\0" 
        lea r00 .@str

        mov r01 0
        .loop:
            geq r02 r01 1000
            cjmp .end r02
            call print_cstr
            inc r01
            jmp .loop

    .end:
        call flush
        exit 0
