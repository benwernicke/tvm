#include "def.h"

static uint64_t instr_bin_map[] = {
    [INSTR_LEA]     = 0x0  << 8,
    [INSTR_MOV]     = 0x1  << 8,
    [INSTR_EXIT]    = 0x2  << 8,
    [INSTR_RET]     = 0x3  << 8,
    [INSTR_CMOV]    = 0x4  << 8,
    [INSTR_JMP]     = 0x5  << 8,
    [INSTR_CJMP]    = 0x6  << 8,
    [INSTR_PUSH]    = 0x7  << 8,
    [INSTR_POP]     = 0x9  << 8,
    [INSTR_STOR]    = 0xa  << 8,
    [INSTR_LOAD]    = 0xb  << 8, 
    [INSTR_ADD]     = 0xc  << 8,
    [INSTR_SUB]     = 0xd  << 8,
    [INSTR_MUL]     = 0xe  << 8,
    [INSTR_DIV]     = 0xf  << 8,
    [INSTR_MOD]     = 0x10 << 8,
    [INSTR_AND]     = 0x11 << 8,
    [INSTR_OR]      = 0x12 << 8,
    [INSTR_XOR]     = 0x13 << 8,
    [INSTR_COMP]    = 0x14 << 8,
    [INSTR_SL]      = 0x15 << 8,
    [INSTR_SR]      = 0x16 << 8,
    [INSTR_CALL]    = 0x17 << 8,
    [INSTR_EQ]      = 0x18 << 8,
    [INSTR_NEQ]     = 0x19 << 8,
    [INSTR_GT]      = 0x1a << 8,
    [INSTR_GEQ]     = 0x1b << 8,
    [INSTR_LT]      = 0x1c << 8,
    [INSTR_LEQ]     = 0x1d << 8,
    [INSTR_NOT]     = 0x1e << 8,
    [INSTR_SYSCALL] = 0x1f << 8,
};

#define ARG_MAX 4

static uint64_t instr_arg_map[][ARG_MAX] = {
    [INSTR_LEA]     = {ARG_RL, ARG_RR},
    [INSTR_MOV]     = {ARG_RL, ARG_RR},
    [INSTR_EXIT]    = {ARG_R, ARG_L},
    [INSTR_RET]     = {ARG_N},
    [INSTR_CMOV]    = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_JMP]     = {ARG_R, ARG_L},
    [INSTR_CJMP]    = {ARG_RR, ARG_LR, ARG_RL, ARG_LL},
    [INSTR_PUSH]    = {ARG_RL, ARG_RR},
    [INSTR_POP]     = {ARG_RL, ARG_RR},
    [INSTR_STOR]    = {ARG_RRL, ARG_RRR},
    [INSTR_LOAD]    = {ARG_RRL, ARG_RRR},
    [INSTR_ADD]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_SUB]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_MUL]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_DIV]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_MOD]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_AND]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_OR]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_XOR]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_COMP]    = {ARG_RL, ARG_RR},
    [INSTR_SL]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_SR]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_CALL]    = {ARG_R, ARG_L},
    [INSTR_EQ]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_NEQ]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_GT]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_GEQ]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_LT]      = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_LEQ]     = {ARG_RLR, ARG_RRR, ARG_RLL, ARG_RRL},
    [INSTR_NOT]     = {ARG_RL, ARG_RR},
    [INSTR_SYSCALL] = {ARG_N},
};

uint64_t instr_bin(instr_t i)
{
    return instr_bin_map[i];
}

uint64_t* instr_arg(instr_t i)
{
    return instr_arg_map[i];
}
