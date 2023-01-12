#ifndef DEF_H
#define DEF_H

#include <stdint.h>

#define INSTR_ARG_MAX 4

#define INSTR_MASK 0xFFFFFFFFFFFFFF00UL
#define ARG_MASK   0x00000000000000FFUL

typedef enum {
    INSTR_MOV = 0,
    INSTR_CMOV,
    INSTR_LEA,
    INSTR_JMP,
    INSTR_CJMP,
    INSTR_CALL,
    INSTR_RETURN,
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
    INSTR_SL,
    INSTR_SR,
    INSTR_EQ,
    INSTR_NEQ,
    INSTR_GT,
    INSTR_GEQ,
    INSTR_LT,
    INSTR_LEQ,
    INSTR_NOT,
    INSTR_COMP,
    INSTR_EXIT,
    INSTR_SYSCALL,
    INSTR_PRNT,
} instr_t;

#define INSTR_NUM 32

typedef enum {
    R00 = 0,
    R01,
    R02,
    R03,
    R04,
    R05,
    R06,
    R07,
    R08,
    R09,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RIP,
    RSP,
    RET,
} reg_t;

#define REG_NUM 19

typedef enum {
    ARG_N = 0,
    ARG_L,
    ARG_R,
    ARG_LL,
    ARG_RL,
    ARG_LR,
    ARG_RR,
    ARG_LLR,
    ARG_RLR,
    ARG_LRR,
    ARG_RRR,
    ARG_LLL,
    ARG_RLL,
    ARG_LRL,
    ARG_RRL,
} arg_t;

#define ARG_NUM 15

extern const char*    reg_str_map[];
extern const uint64_t reg_bin_map[];

extern const char*    instr_str_map[];
extern const uint64_t instr_bin_map[];
extern const arg_t    instr_arg_map[][INSTR_ARG_MAX];
extern const uint64_t instr_arg_num_map[];
extern const uint64_t instr_arg_max_permutation[];

extern const char*    arg_str_map[];
extern const uint64_t arg_bin_map[];

#endif
