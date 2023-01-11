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

#define R00 ((uint64_t)0)
#define R01 ((uint64_t)1)
#define R02 ((uint64_t)2)
#define R03 ((uint64_t)3)
#define R04 ((uint64_t)4)
#define R05 ((uint64_t)5)
#define R06 ((uint64_t)6)
#define R07 ((uint64_t)7)
#define R08 ((uint64_t)8)
#define R09 ((uint64_t)9)
#define R10 ((uint64_t)10)
#define R11 ((uint64_t)11)
#define R12 ((uint64_t)12)
#define R13 ((uint64_t)13)
#define R14 ((uint64_t)14)
#define R15 ((uint64_t)15)
#define RIP ((uint64_t)16)
#define RSP ((uint64_t)17)
#define RET ((uint64_t)18)

uint64_t  instr_bin(instr_t a);
uint64_t* instr_arg(instr_t i);

#endif
