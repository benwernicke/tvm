#ifndef DEF_H
#define DEF_H

#include <stdint.h>

// must start at 1 to ensure marking end with 0 works
#define ARG_N    0x1  
#define ARG_R    0x2  
#define ARG_L    0x3  
#define ARG_RR   0x4  
#define ARG_LR   0x5  
#define ARG_RL   0x6  
#define ARG_LL   0x7  
#define ARG_RRL  0x8  
#define ARG_LRL  0x9  
#define ARG_RLL  0xa  
#define ARG_LLL  0xb  
#define ARG_RRR  0xc  
#define ARG_LRR  0xd  
#define ARG_RLR  0xe  
#define ARG_LLR  0xf

typedef enum {
    INSTR_LEA = 0,
    INSTR_MOV,
    INSTR_EXIT,
    INSTR_RET,
    INSTR_CMOV,
    INSTR_JMP,
    INSTR_CJMP,
    INSTR_PUSH,
    INSTR_POP,
    INSTR_STOR,
    INSTR_LOAD,
    INSTR_ADD,
    INSTR_SUB,
    INSTR_MUL,
    INSTR_DIV,
    INSTR_MOD,
    INSTR_AND,
    INSTR_OR,
    INSTR_XOR,
    INSTR_COMP,
    INSTR_SL,
    INSTR_SR,
    INSTR_CALL,
    INSTR_EQ,
    INSTR_NEQ,
    INSTR_GT,
    INSTR_GEQ,
    INSTR_LT,
    INSTR_LEQ,
    INSTR_NOT,
    INSTR_SYSCALL,
} instr_t;

uint64_t  instr_bin(instr_t a);
uint64_t* instr_arg(instr_t i);

#endif
