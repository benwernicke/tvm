main:
    .@newline: char
    lea r00 .@newline
    stor r00 '\n' 1
    prnt 'A'
    lea r01 .@newline
    load r00 r01 1
    prnt r00
    exit 0
