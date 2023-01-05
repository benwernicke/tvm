#ifndef INSTR_H
#define INSTR_H

#include <stdint.h>

typedef enum {
    INSTR_MOV,
    INSTR_CMOV,
    INSTR_CALL,
    INSTR_RET,
    INSTR_ADD,
    INSTR_SUB,
    INSTR_MUL,
    INSTR_DIV,
    INSTR_MOD,
    INSTR_COMP,
    INSTR_AND,
    INSTR_OR,
    INSTR_XOR,
    INSTR_SR,
    INSTR_SL,
    INSTR_NOT,
    INSTR_EQ,
    INSTR_GT,
    INSTR_LT,
    INSTR_LOAD,
    INSTR_STOR,
    INSTR_PRNT,
    INSTR_ALOC,
    INSTR_FREE,
    INSTR_EXIT
} instr_type_t;

typedef enum {
    ARG_R,
    ARG_I,
    ARG_RR,
    ARG_IR,
    ARG_RI,
    ARG_II,
    ARG_RRR,
    ARG_IRR,
    ARG_RIR,
    ARG_IIR,
    ARG_RRI,
    ARG_IRI,
    ARG_RII,
    ARG_III,

} arg_type_t;

typedef struct instr_t instr_t;
struct instr_t {
    uint32_t a[3];

    instr_type_t type;
    arg_type_t arg_type;
};

#endif // INSTR_H
