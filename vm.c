#include <stdint.h>

#include "instr.h"

typedef enum {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_ILLEGAL_INSTRUCTION,
    TRAP_ILLEGAL_REGISTER,
} trap_t;

bool is_not_valid_reg(uintptr_t a) { 
}

static inline trap_t instr_resolve_mov(instr_t* i)
{
    uint64_t* a;
    uint64_t* b;
    uint64_t* c;
    switch (i->arg_type) {
    case ARG_RRR:
        return TRAP_OK;
    case ARG_RIR:
        return TRAP_OK;
    case ARG_RRI:
        return TRAP_OK;
    case ARG_RII:
        return TRAP_OK;
    default:
    case ARG_R:
    case ARG_I:
    case ARG_RR:
    case ARG_IR:
    case ARG_RI:
    case ARG_II:
    case ARG_III:
    case ARG_IRI:
    case ARG_IIR:
    case ARG_IRR:
        return TRAP_ILLEGAL_INSTRUCTION_ARGUMENTS;
    }
}
static inline trap_t instr_resolve_cmov(instr_t* i);
static inline trap_t instr_resolve_call(instr_t* i);
static inline trap_t instr_resolve_ret(instr_t* i);
static inline trap_t instr_resolve_add(instr_t* i);
static inline trap_t instr_resolve_sub(instr_t* i);
static inline trap_t instr_resolve_mul(instr_t* i);
static inline trap_t instr_resolve_div(instr_t* i);
static inline trap_t instr_resolve_mod(instr_t* i);
static inline trap_t instr_resolve_comp(instr_t* i);
static inline trap_t instr_resolve_and(instr_t* i);
static inline trap_t instr_resolve_or(instr_t* i);
static inline trap_t instr_resolve_xor(instr_t* i);
static inline trap_t instr_resolve_sr(instr_t* i);
static inline trap_t instr_resolve_sl(instr_t* i);
static inline trap_t instr_resolve_not(instr_t* i);
static inline trap_t instr_resolve_eq(instr_t* i);
static inline trap_t instr_resolve_gt(instr_t* i);
static inline trap_t instr_resolve_lt(instr_t* i);
static inline trap_t instr_resolve_load(instr_t* i);
static inline trap_t instr_resolve_stor(instr_t* i);
static inline trap_t instr_resolve_prnt(instr_t* i);
static inline trap_t instr_resolve_aloc(instr_t* i);
static inline trap_t instr_resolve_free(instr_t* i);
static inline trap_t instr_resolve_exi(instr_t* i);

static trap_t instr_resolve(instr_t* i)
{
    switch (i->type) {
    case INSTR_MOV:
        return instr_resolve_mov(i);
    case INSTR_CMOV:
        return instr_resolve_cmov(i);
    case INSTR_CALL:
        return instr_resolve_call(i);
    case INSTR_RET:
        return instr_resolve_ret(i);
    case INSTR_ADD:
        return instr_resolve_add(i);
    case INSTR_SUB:
        return instr_resolve_sub(i);
    case INSTR_MUL:
        return instr_resolve_mul(i);
    case INSTR_DIV:
        return instr_resolve_div(i);
    case INSTR_MOD:
        return instr_resolve_mod(i);
    case INSTR_COMP:
        return instr_resolve_comp(i);
    case INSTR_AND:
        return instr_resolve_and(i);
    case INSTR_OR:
        return instr_resolve_or(i);
    case INSTR_XOR:
        return instr_resolve_xor(i);
    case INSTR_SR:
        return instr_resolve_sr(i);
    case INSTR_SL:
        return instr_resolve_sl(i);
    case INSTR_NOT:
        return instr_resolve_not(i);
    case INSTR_EQ:
        return instr_resolve_eq(i);
    case INSTR_GT:
        return instr_resolve_gt(i);
    case INSTR_LT:
        return instr_resolve_lt(i);
    case INSTR_LOAD:
        return instr_resolve_load(i);
    case INSTR_STOR:
        return instr_resolve_stor(i);
    case INSTR_PRNT:
        return instr_resolve_prnt(i);
    case INSTR_ALOC:
        return instr_resolve_aloc(i);
    case INSTR_FREE:
        return instr_resolve_free(i);
    case INSTR_EXI:
        return instr_resolve_exi(i);
    default:
        return TRAP_ILLEGAL_OPERATION;
    }
}
