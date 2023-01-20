#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "def.h"
#include "err.h"
#include "flag.h"

#define STACK_SIZE 4096

static uint64_t reg[REG_NUM] = { 0 };
static char stack[STACK_SIZE] = { 0 };

typedef struct program_t program_t;
struct program_t {
    uint64_t  code_size;
    uint64_t* code;
    uint64_t  data_size;
    char*     data;
};

static program_t program = {0};

static void load_file(char* path)
{
    FILE* f = fopen(path, "rb");
    ERR_IF(!f, ERR_FILE);
    fread(&program.code_size, 1, sizeof(program.code_size), f);
    fread(&program.data_size, 1, sizeof(program.data_size), f);
    program.code = malloc(program.code_size * sizeof(*program.code));
    ERR_IF(!program.code, ERR_BAD_MALLOC);
    fread(program.code, program.code_size, sizeof(*program.code), f);

    if (program.data_size) {
        program.data = malloc(program.data_size * sizeof(*program.data));
        ERR_IF(!program.data, ERR_BAD_MALLOC);
        fread(program.data, program.data_size, sizeof(*program.data), f);
    }
    fclose(f);

error:
    return;
}

static instr_t instr_from_bin(uint64_t b)
{
    const uint64_t* iter = instr_bin_map;
    const uint64_t* end  = instr_bin_map + INSTR_NUM;
    for (; iter != end; ++iter) {
        if (*iter == b) {
            break;
        }
    }
    return iter - instr_bin_map;
}

static reg_t reg_from_bin(uint64_t b)
{
    const uint64_t* iter = reg_bin_map;
    const uint64_t* end  = reg_bin_map + REG_NUM;
    for (; iter != end; ++iter) {
        if (*iter == b) {
            break;
        }
    }
    return iter - reg_bin_map;
}

void arg_format(arg_t arg_type, uint64_t* arg_bin)
{
    switch (arg_type) {
    case ARG_N: 
        break;
    case ARG_L: 
        printf("%lu", 
                arg_bin[0]
            );
        break;
    case ARG_R: 
        printf("%s",
                reg_str_map[reg_from_bin(arg_bin[0])]
            );
        break;
    case ARG_LL: 
        printf("%lu %lu",
               arg_bin[0],
               arg_bin[1]
            );
        break;
    case ARG_RL: 
        printf("%s %lu",
                reg_str_map[reg_from_bin(arg_bin[0])],
                arg_bin[1]
            );
        break;
    case ARG_LR: 
        printf("%lu %s",
                arg_bin[0],
                reg_str_map[reg_from_bin(arg_bin[1])]
            );
        break;
    case ARG_RR: 
        printf("%s %s",
                reg_str_map[reg_from_bin(arg_bin[0])],
                reg_str_map[reg_from_bin(arg_bin[1])]
            );
        break;
    case ARG_LLR: 
        printf("%lu %lu %s",
                arg_bin[0],
                arg_bin[1],
                reg_str_map[reg_from_bin(arg_bin[2])]
            );
        break;
    case ARG_RLR: 
        printf("%s %lu %s",
                reg_str_map[reg_from_bin(arg_bin[0])],
                arg_bin[1],
                reg_str_map[reg_from_bin(arg_bin[2])]
            );
        break;
    case ARG_LRR: 
        printf("%lu %s %s",
                arg_bin[0],
                reg_str_map[reg_from_bin(arg_bin[1])],
                reg_str_map[reg_from_bin(arg_bin[2])]
            );
        break;
    case ARG_RRR: 
        printf("%s %s %s",
                reg_str_map[reg_from_bin(arg_bin[0])],
                reg_str_map[reg_from_bin(arg_bin[1])],
                reg_str_map[reg_from_bin(arg_bin[2])]
            );
        break;
    case ARG_LLL: 
        printf("%lu %lu %lu",
               arg_bin[0],
               arg_bin[1],
               arg_bin[2]
            );
        break;
    case ARG_RLL: 
        printf("%s %lu %lu",
                reg_str_map[reg_from_bin(arg_bin[0])],
                arg_bin[1],
                arg_bin[2]
            );
        break;
    case ARG_LRL: 
        printf("%lu %s %lu",
                arg_bin[0],
                reg_str_map[reg_from_bin(arg_bin[1])],
                arg_bin[2]
            );
        break;
    case ARG_RRL: 
        printf("%s %s %lu",
                reg_str_map[reg_from_bin(arg_bin[0])],
                reg_str_map[reg_from_bin(arg_bin[1])],
                arg_bin[2]
            );
        break;
    }
}

static void dump_asm(void)
{
    uint64_t i;
    for (i = 0; i < program.code_size; ++i) {
        instr_t  instr      = instr_from_bin(program.code[i] & INSTR_MASK);
        arg_t    arg_type   = program.code[i] & ARG_MASK;
        uint64_t arg_bin[3] = {0};

        uint64_t j;
        for (j = 0; j < instr_arg_num_map[instr]; ++j) {
            arg_bin[j] = program.code[++i];
        }
        printf("%s\r\t", instr_str_map[instr]);
        arg_format(arg_type, arg_bin);
        printf("\n");
    }
}

static bool run_curr_instr(uint64_t* exit_code)
{
    instr_t instr = instr_from_bin(program.code[reg[RIP]] & INSTR_MASK);
    arg_t   arg   = program.code[reg[RIP]] & ARG_MASK;

    uint64_t* a = NULL;
    uint64_t* b = NULL;
    uint64_t* c = NULL;

    switch (arg) {
    case ARG_N: break;
    case ARG_L: 
        a = &program.code[++reg[RIP]];
        break;
    case ARG_R: 
        a = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_LL: 
        a = &program.code[++reg[RIP]];
        b = &program.code[++reg[RIP]];
        break;
    case ARG_RL: 
        a = &reg[program.code[++reg[RIP]]];
        b = &program.code[++reg[RIP]];
        break;
    case ARG_LR:
        a = &program.code[++reg[RIP]];
        b = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_RR:
        a = &reg[program.code[++reg[RIP]]];
        b = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_LLR:
        a = &program.code[++reg[RIP]];
        b = &program.code[++reg[RIP]];
        c = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_RLR:
        a = &program.code[++reg[RIP]];
        b = &reg[program.code[++reg[RIP]]];
        c = &program.code[++reg[RIP]];
        break;
    case ARG_LRR:
        a = &program.code[++reg[RIP]];
        b = &reg[program.code[++reg[RIP]]];
        c = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_RRR:
        a = &reg[program.code[++reg[RIP]]];
        b = &reg[program.code[++reg[RIP]]];
        c = &reg[program.code[++reg[RIP]]];
        break;
    case ARG_LLL:
        a = &program.code[++reg[RIP]];
        b = &program.code[++reg[RIP]];
        c = &program.code[++reg[RIP]];
        break;
    case ARG_RLL:
        a = &reg[program.code[++reg[RIP]]];
        b = &program.code[++reg[RIP]];
        c = &program.code[++reg[RIP]];
        break;
    case ARG_LRL:
        a = &program.code[++reg[RIP]];
        b = &reg[program.code[++reg[RIP]]];
        c = &program.code[++reg[RIP]];
        break;
    case ARG_RRL:
        a = &reg[program.code[++reg[RIP]]];
        b = &reg[program.code[++reg[RIP]]];
        c = &program.code[++reg[RIP]];
        break;
    default:
        ERR(ERR_VM);
    }

    switch (instr) {
    case INSTR_INC:
            *a += 1;
        break;
    case INSTR_DEC:
            *a -= 1;
        break;
    case INSTR_MOV:
        *a = *b;
        break;
    case INSTR_CMOV:
        if (*c) {
            *a = *b;
        }
        break;
    case INSTR_LEA:
        if (*b >= program.data_size) {
            fprintf(stderr, "Error: static storage overflow\n");
            ERR(ERR_VM);
        }
        *a = (uint64_t)(program.data + *b);
        break;
    case INSTR_JMP:
        reg[RIP] = *a;
        break;
    case INSTR_CJMP:
        if (*b) {
            reg[RIP] = *a;
        }
        break;
    case INSTR_CALL:
        reg[RET] = reg[RIP];
        reg[RIP] = *a;
        break;
    case INSTR_RETURN:
        reg[RIP] = reg[RET];
        break;
    case INSTR_PUSH:
        if ((char*)(reg[RSP] - 8) < stack) {
            fprintf(stderr, "Error: stack overflow\n");
            ERR(ERR_VM);
        }
        reg[RSP] -= 8;
        *(uint64_t*)reg[RSP] = *a;
        break;
    case INSTR_POP:
        if ((char*)(reg[RSP] + 8) > stack + STACK_SIZE) {
            fprintf(stderr, "Error: stack underflow\n");
            ERR(ERR_VM);
        }

        *a = *(uint64_t*)reg[RSP];
        reg[RSP] += 8;
        break;
    case INSTR_STOR:
        memcpy((void*)*a, b, *c);
        break;
    case INSTR_LOAD:
        // matching required too allow for:
            // load r00 r00 8
            // load r00 r01 r00
        if (a == b) {
            void* t = (void*)*b;
            *a = 0;
            memcpy(a, t, *c);
        } else if (a == c) {
            uint64_t t = *c;
            *a = 0;
            memcpy(a, (void*)*b, t);
        } else {
            *a = 0;     // if less than 8 byte loaded it should be that value
            memcpy(a, (void*)*b, *c);
        }
        break;
    case INSTR_ADD:
        *a = *b + *c;
        break;
    case INSTR_SUB:
        *a = *b - *c;
        break;
    case INSTR_MUL:
        *a = *b * *c;
        break;
    case INSTR_DIV:
        *a = *b / *c;
        break;
    case INSTR_MOD:
        *a = *b % *c;
        break;
    case INSTR_AND:
        *a = *b & *c;
        break;
    case INSTR_OR:
        *a = *b | *c;
        break;
    case INSTR_XOR:
        *a = *b ^ *c;
        break;
    case INSTR_SL:
        *a = *b << *c;
        break;
    case INSTR_SR:
        *a = *b >> *c;
        break;
    case INSTR_EQ:
        *a = *b == *c;
        break;
    case INSTR_NEQ:
        *a = *b != *c;
        break;
    case INSTR_GT:
        *a = *b > *c;
        break;
    case INSTR_GEQ:
        *a = *b >= *c;
        break;
    case INSTR_LT:
        *a = *b < *c;
        break;
    case INSTR_LEQ:
        *a = *b <= *c;
        break;
    case INSTR_NOT:
        *a = !*b;
        break;
    case INSTR_COMP:
        *a = ~*b;
        break;
    case INSTR_EXIT:
        *exit_code = *a;
        return 0;
    case INSTR_SYSCALL:
        reg[R00] = syscall(reg[R00], reg[R01], reg[R02], reg[R03], reg[R04], reg[R05]);
        break;
    case INSTR_PRNT:
        putc(*a, stdout);
        break;
    default:
        ERR(ERR_VM);
    }

    return 1;

error:
    return 0;
}

static void run(void)
{
    bool running = 1;
    uint64_t exit_code = 0;
    for (; running && reg[RIP] < program.code_size; ++reg[RIP]) {
        running = run_curr_instr(&exit_code);
        ERR_FORWARD();
    }

    if (exit_code != 0) {
        fprintf(stderr, "Error: exited with non zero exit code: %lu\n", exit_code);
    }
error:
    return;
}

void format_curr_instr(void)
{
    instr_t instr = instr_from_bin(program.code[reg[RIP]] & INSTR_MASK);
    arg_t   arg   = program.code[reg[RIP]] & ARG_MASK;

    printf("$ %s", instr_str_map[instr]);

    switch (arg) {
    case ARG_N: break;
    case ARG_L: 
        printf(" %lu", program.code[reg[RIP] + 1]);
        break;
    case ARG_R: 
        printf(" %s", 
                reg_str_map[program.code[reg[RIP] + 1]]
            );
        break;
    case ARG_LL: 
        printf(" %lu %lu", 
                program.code[reg[RIP] + 1],
                program.code[reg[RIP] + 2]
            );
        break;
    case ARG_RL: 
        printf(" %s %lu", 
                reg_str_map[program.code[reg[RIP] + 1]],
                program.code[reg[RIP] + 2]
            );
        break;
    case ARG_LR:
        printf(" %lu %s",
                program.code[reg[RIP] + 1],
                reg_str_map[program.code[reg[RIP] + 2]]
            );
        break;
    case ARG_RR:
        printf(" %s %s",
                reg_str_map[program.code[reg[RIP] + 1]],
                reg_str_map[program.code[reg[RIP] + 2]]
            );
        break;
    case ARG_LLR:
        printf(" %lu %lu %s",
                program.code[reg[RIP] + 1],
                program.code[reg[RIP] + 2],
                reg_str_map[program.code[reg[RIP] + 3]]
            );
        break;
    case ARG_RLR:
        printf(" %s %lu %s",
                reg_str_map[program.code[reg[RIP] + 1]],
                program.code[reg[RIP] + 2],
                reg_str_map[program.code[reg[RIP] + 3]]
            );
        break;
    case ARG_LRR:
        printf(" %lu %s %s",
                program.code[reg[RIP] + 1],
                reg_str_map[program.code[reg[RIP] + 2]],
                reg_str_map[program.code[reg[RIP] + 3]]
            );
        break;
    case ARG_RRR:
        printf(" %s %s %s",
                reg_str_map[program.code[reg[RIP] + 1]],
                reg_str_map[program.code[reg[RIP] + 2]],
                reg_str_map[program.code[reg[RIP] + 3]]
            );
        break;
    case ARG_LLL:
        printf(" %lu %lu %lu",
                program.code[reg[RIP] + 1],
                program.code[reg[RIP] + 2],
                program.code[reg[RIP] + 3]
            );
        break;
    case ARG_RLL:
        printf(" %s %lu %lu",
                reg_str_map[program.code[reg[RIP] + 1]],
                program.code[reg[RIP] + 2],
                program.code[reg[RIP] + 3]
            );
        break;
    case ARG_LRL:
        printf(" %lu %s %lu",
                program.code[reg[RIP] + 1],
                reg_str_map[program.code[reg[RIP] + 2]],
                program.code[reg[RIP] + 3]
            );
        break;
    case ARG_RRL:
        printf(" %s %s %lu",
                reg_str_map[program.code[reg[RIP] + 1]],
                reg_str_map[program.code[reg[RIP] + 2]],
                program.code[reg[RIP] + 3]
            );
        break;
    default:
        ERR(ERR_VM);
    }

    printf("\n\n");

error:
    return;
}

static char* triml(char* s)
{
    for (; isspace(*s); ++s) {  }
    return s;
}

static char* trim(char* s)
{
    s = triml(s);
    if (!*s) return s;
    char* e = s;
    for (; *e; ++e) {  }
    e -= 1;
    for (; e != s && isspace(*e); --e) {  }
    e[1] = 0;
    return s;
}

void debugger(void) 
{
    uint64_t exit_code = 0;
    bool running = 1;

    char* cmd = NULL;
    uint64_t cmd_cap = 0;

    for (; running && reg[RIP] < program.code_size; ++reg[RIP]) {
        format_curr_instr();
        while (1) {
            printf("> ");
            getline(&cmd, &cmd_cap, stdin);
            char* s = trim(cmd);

            if (!*s) {
                continue;
            } else if (strcmp(s, "n") == 0) {
                break;
            } else if (strncmp(s, "p", 1) == 0) {
                s = triml(++s);
                const char** reg_iter = reg_str_map;
                const char** reg_end  = reg_str_map + REG_NUM;
                for (; reg_iter != reg_end; ++reg_iter) {
                    if (strcmp(s, *reg_iter) == 0) {
                        break;
                    }
                }
                if (reg_iter == reg_end) {
                    printf("! Reg: '%s' unknown\n\n", s);
                } else {
                    printf("%s: %lu\n\n", s, reg[reg_iter - reg_str_map]);
                }
            } else if (strcmp(s, "q") == 0) {
                goto end;
            } else {
                printf("! Cmd: '%s' unknown\n\n", s);
            }
        }
        running = run_curr_instr(&exit_code);
        ERR_FORWARD();
    }

end:

    free(cmd);

    return;

error:
    free(cmd);
    fprintf(stderr, "Error: some error in vm debugger\n");
    return;
}

static void dump_reg(void)
{
    uint64_t i = 0;
    for (; i < REG_NUM; ++i) {
        printf("<%s>: %lu\n", reg_str_map[i], reg[i]);
    }
}

void program_deinit(void)
{
    free(program.code);
}

static bool flag_help      = 0;
static bool flag_dump_asm  = 0;
static bool flag_dump_reg  = 0;
static bool flag_dbg       = 0;

static flag_t flags[] = {
    {
        .short_identifier = 'h',
        .long_identifier  = "help",
        .description      = "show this page and exit",
        .target           = &flag_help,
        .type             = FLAG_BOOL,
    },
    {
        .short_identifier = 0,
        .long_identifier  = "asm",
        .description      = "dump the assembly and exit",
        .target           = &flag_dump_asm,
        .type             = FLAG_BOOL,
    },
    {
        .short_identifier = 0,
        .long_identifier  = "reg",
        .description      = "dump the register after running and exit",
        .target           = &flag_dump_reg,
        .type             = FLAG_BOOL,
    },
    {
        .short_identifier = 0,
        .long_identifier  = "dbg",
        .description      = "open debugger for given program",
        .target           = &flag_dbg,
        .type             = FLAG_BOOL,
    },
};

static uint32_t flags_len = sizeof(flags) / sizeof(*flags);

int main(int argc, char** argv)
{
    reg[RSP] = (uint64_t)(stack + STACK_SIZE);
    // flag stuff
    {
        int e = flag_parse(argc, argv, flags, flags_len, &argc, &argv);
        if (e) {
            fprintf(stderr, "Error in flag parsing\n");
            fprintf(stderr, "\t%s at %s", flag_error_format(e), *flag_error_position());
            exit(1);
        }

        if (flag_help) {
            flag_print_usage(stdout, "tvm a tiny vm", flags, flags_len);
            goto end;
        }
    }

    load_file(argv[1]);

    if (err_get() != ERR_OK) {
        err_format(stderr);
        goto error;
    }

    if (flag_dump_asm) {
        dump_asm();
        goto end;
    }

    if (flag_dbg) {
        debugger();
        goto end;
    }

    if (flag_dump_reg) {
        run();
        dump_reg();

        if (err_get() != ERR_OK) {
            err_format(stderr);
            goto error;
        }

        goto end;
    }

    run();

end:
    program_deinit();
    return 0;

error:
    program_deinit();
    return 1;
}
