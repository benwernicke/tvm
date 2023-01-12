#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "def.h"
#include "err.h"
#include "flag.h"
#include "label_map.h"
#include "label_buf.h"

typedef struct asm_t asm_t;
struct asm_t {
    uint64_t* code;
    uint64_t  code_size;
    uint64_t  code_cap;
    label_map_t* label_map;
    label_buf_t* label_buf;
};

static uint64_t* asm_more(asm_t* a)
{
    if (a->code_size >= a->code_cap) {
        a->code_cap += 1;
        a->code_cap <<= 1;
        a->code = realloc(a->code, a->code_cap * sizeof(*a->code));
        ERR_IF(!a->code, ERR_BAD_MALLOC);
    }

    return &a->code[a->code_size++];

error:
    return NULL;
}

static char* trim_comments(uint64_t* line_number, char* s)
{
    for (; isspace(*s); ++s) {
        if (*s == '\n') *line_number += 1;
    }
    if (*s == '#') {
        for (; *s && *s != '\n'; ++s) {}
        return trim_comments(line_number, s);
    }
    return s;
}

static void get_token(uint64_t* line_number, uint64_t* ip, char* s, char** start, char** end)
{
    *ip += 1;
    s = trim_comments(line_number, s);
    if (!*s) {
        *start = NULL;
    }
    *start = s;
    for (; *s && !isspace(*s); ++s) {
        if (*s == '\n') *line_number += 1;
    }
    *end = s;
}

static bool token_is_literal(char* s, char* e)
{
    for (; isdigit(*s) && s != e; ++s) {  }
    return s == e;
}

static bool token_is_register(char* s, char* e)
{
    const char** reg_iter = reg_str_map;
    const char** reg_end = reg_str_map + REG_NUM;
    char t = *e;
    *e = 0;
    for (; reg_iter != reg_end; ++reg_iter) {
        if (strcmp(*reg_iter, s) == 0) {
            break;
        }
    }
    *e = t;
    return reg_iter != reg_end;
}

static bool token_is_instruction(char* s, char* e)
{
    const char** instr_iter = instr_str_map;
    const char** instr_end = instr_str_map + INSTR_NUM;
    char t = *e;
    *e = 0;
    for (; instr_iter != instr_end; ++instr_iter) {
        if (strcmp(s, *instr_iter) == 0) {
            break;
        }
    }
    *e = t;
    return instr_iter != instr_end;
}

static instr_t token_get_instruction(char* s, char* e)
{
    const char** instr_iter = instr_str_map;
    const char** instr_end = instr_str_map + INSTR_NUM;
    char t = *e;
    *e = 0;
    for (; instr_iter != instr_end; ++instr_iter) {
        if (strcmp(s, *instr_iter) == 0) {
            break;
        }
    }
    *e = t;
    return instr_iter - instr_str_map;
}

static bool token_is_label(char* s, char* e)
{
    return *(e - 1) == ':';
}

static uint64_t token_get_register_bin(char* s, char* e)
{
    char t = *e;
    *e = 0;
    const char** reg_iter = reg_str_map;
    const char** reg_end  = reg_str_map + REG_NUM;
    for (; reg_iter != reg_end; ++reg_iter) {
        if (strcmp(*reg_iter, s) == 0) {
            break;
        }
    }
    *e = t;
    return reg_bin_map[reg_iter - reg_str_map];
}

static uint64_t token_get_literal(char* s, char* e)
{
    uint64_t sum = 0;
    for (; s != e; ++s) {
        sum *= 10;
        sum += (*s - '0');
    }
    return sum;
}

static arg_t build_arg(int* a)
{
    // 0 args
    if (a[0] == -1) return ARG_N;

    // 1 arg
    if (a[0] == 0 && a[1] == -1) return ARG_L;
    if (a[0] == 1 && a[1] == -1) return ARG_R;

    // 2 args
    if (a[0] == 0 && a[1] == 0 && a[2] == -1) return ARG_LL;
    if (a[0] == 0 && a[1] == 1 && a[2] == -1) return ARG_LR;
    if (a[0] == 1 && a[1] == 0 && a[2] == -1) return ARG_RL;
    if (a[0] == 1 && a[1] == 1 && a[2] == -1) return ARG_RR;

    // 3 args
    if (a[0] == 0 && a[1] == 0 && a[2] == 0) return ARG_LLL;
    if (a[0] == 0 && a[1] == 1 && a[2] == 0) return ARG_LRL;
    if (a[0] == 1 && a[1] == 0 && a[2] == 0) return ARG_RLL;
    if (a[0] == 1 && a[1] == 1 && a[2] == 0) return ARG_RRL;
    if (a[0] == 0 && a[1] == 0 && a[2] == 1) return ARG_LLR;
    if (a[0] == 0 && a[1] == 1 && a[2] == 1) return ARG_LRR;
    if (a[0] == 1 && a[1] == 0 && a[2] == 1) return ARG_RLR;
    if (a[0] == 1 && a[1] == 1 && a[2] == 1) return ARG_RRR;
    __builtin_unreachable();
}

static void asm_free(asm_t* a)
{
    if (a) {
        {
            uintptr_t iter = label_map_begin(a->label_map);
            for (; iter != label_map_end(a->label_map); iter = label_map_next(a->label_map, iter) ) {
                free(*label_map_iter_key(a->label_map, iter));
            }
            label_map_free(a->label_map);
        }
        {
            label_buf_pair_t* iter = label_buf_begin(a->label_buf);
            label_buf_pair_t* end  = label_buf_end(a->label_buf);
            for (; iter != end; ++iter) {
                free(iter->label);
            }
            label_buf_free(a->label_buf);
        }
        free(a->code);
        free(a);
    }
}

static void* rptr_create(void* b, void* t)
{
    return (void*)((char*)t - (char*)b - 1);
}

static void* rptr_get(void* b, void* t)
{
    return (void*)((uintptr_t)b + (uintptr_t)t + 1);
}

static asm_t* asm_from_file(char* path)
{
    char*    file_content = NULL;
    uint64_t line_number  = 1;
    asm_t* a = calloc(1, sizeof(*a));
    ERR_IF(!a, ERR_BAD_MALLOC);
    a->label_map = label_map_create();
    a->label_buf = label_buf_create();
    ERR_IF(!a->label_map || !a->label_buf, ERR_BAD_MALLOC);

    // read file
    {
        FILE* f = fopen(path, "r");
        ERR_IF(!f, ERR_FILE);
        fseek(f, 0, SEEK_END);
        uint64_t l = ftell(f);
        rewind(f);
        file_content = malloc(l + 1);
        ERR_IF(!file_content, ERR_BAD_MALLOC);
        file_content[l] = 0;
        fread(file_content, 1, l, f);
        fclose(f);
    }

    uint64_t ip         = 0;
    char*    last_label = NULL;
    char* s;
    char* e;
    get_token(&line_number, &ip, file_content, &s, &e);
    if (!s) {
        fprintf(stderr, "Error: file 's' seems to be empty\n");
        ERR(ERR_ASSEMBLER);
    }

    do {
        if (token_is_instruction(s, e)) {
            instr_t  instr              = token_get_instruction(s, e);
            int      arg_types[3]       = { -1, -1, -1 };
            label_buf_pair_t* labels[3] = { 0 };
            uint64_t args[3]            = { 0 };
            uint64_t i                  = 0;
            for (; i < instr_arg_num_map[instr]; ++i) {
                get_token(&line_number, &ip, e, &s, &e);
                ERR_IF(!s, ERR_ASSEMBLER);

                if (token_is_literal(s, e)) {
                    arg_types[i] = 0;
                    args[i] = token_get_literal(s, e);

                } else if (token_is_register(s, e)) {
                    arg_types[i] = 1;
                    args[i] = token_get_register_bin(s, e);

                } else {
                    arg_types[i] = 0;

                    label_buf_pair_t* p = label_buf_more(a->label_buf);
                    ERR_IF(!p, ERR_BAD_MALLOC);

                    char* l   = NULL;
                    labels[i] = rptr_create(a->label_buf->buf, p);

                    if (*s == ':') {
                        uint64_t len = strlen(path) + (e - s) + 1;
                        l = calloc(1, len);
                        ERR_IF(!l, ERR_BAD_MALLOC);
                        strcpy(l, path);
                        strncat(l, s, e - s);
                    } else  if (*s == '.') {
                        uint64_t len = strlen(last_label) + (e - s) + 1;
                        l = calloc(1, len);
                        ERR_IF(!l, ERR_BAD_MALLOC);
                        strcpy(l, last_label);
                        strncat(l, s, e - s);
                    } else {
                        l = calloc(1, (e - s) + 1);
                        ERR_IF(!l, ERR_BAD_MALLOC);
                        strncpy(l, s, e - s);
                    }
                    p->label = l;
                }
            }
            uint64_t* n = asm_more(a);
            ERR_FORWARD();
            arg_t arg = build_arg(arg_types);

            for (i = 0; i < instr_arg_max_permutation[instr]; ++i) {
                if (arg == instr_arg_map[instr][i]) {
                    break;
                }
            }

            if (i == instr_arg_max_permutation[instr]) {
                fprintf(stderr, "Error: '%s' is not a valid arg form for '%s'\n", 
                        arg_str_map[arg], 
                        instr_str_map[instr]
                    );
                fprintf(stderr, "\tvalid forms would be:\n");
                for (i = 0; i < instr_arg_max_permutation[instr]; ++i) {
                    fprintf(stderr, "\t\t%s %s", 
                            instr_str_map[instr], 
                            arg_str_map[instr_arg_map[instr][i]]
                        );
                }
                fprintf(stderr, "\n\tat line %lu\n", line_number);
                ERR(ERR_ASSEMBLER);
            }

            *n = instr_bin_map[instr] | arg_bin_map[arg];
            for (i = 0; i < instr_arg_num_map[instr]; ++i ) {
                n = asm_more(a);
                ERR_FORWARD();
                *n = args[i];
                if (labels[i]) {
                    ((label_buf_pair_t*)rptr_get(a->label_buf->buf, labels[i]))->pos = rptr_create(a->code, n);
                }
            }
        } else if (token_is_label(s, e)) {
            char* l = NULL;
            e -= 1;
            *e = 0;
            ip -= 1; // get_token advances ip but label is not an instruction

            // file private label
            if(*s == ':') {
                l = malloc(strlen(path) + (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                *l = 0;
                strcat(l, path);
                strcat(l, s);
                int e = label_map_insert(a->label_map, l, ip - 1);
                ERR_IF(e, ERR_BAD_MALLOC);
                last_label = l;

            // function private label
            } else if (*s == '.') {
                l = calloc(1, strlen(last_label) + (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                *l = 0;
                strcat(l, last_label);
                strcat(l, s);
                int e = label_map_insert(a->label_map, l, ip - 1);
                ERR_IF(e, ERR_BAD_MALLOC);

            // file public label
            } else {
                l = calloc(1, (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                strcpy(l, s);
                int e = label_map_insert(a->label_map, l, ip - 1);
                ERR_IF(e, ERR_BAD_MALLOC);
                last_label = l;
            }
            e += 1;
        } else {
            *e = 0;
            fprintf(stderr, "Error: '%s' is not a valid instruction nor label definition\n", s);
            fprintf(stderr, "\tat line %lu\n", line_number);
            ERR(ERR_ASSEMBLER);
        }
        get_token(&line_number, &ip, e, &s, &e);
    } while (s && *s);

    free(file_content);

    // resolve file internal labels
    {
        label_buf_pair_t* iter = label_buf_begin(a->label_buf);
        label_buf_pair_t* end  = label_buf_end(a->label_buf);
        for (; iter != end; ++iter) {
            uint64_t* p = label_map_get(a->label_map, iter->label);
            if (!p) {
                fprintf(stderr, "Error: unknown label '%s'\n", iter->label);
                // TODO line number
                ERR(ERR_ASSEMBLER);
            }
            *(uint64_t*)rptr_get(a->code, iter->pos) = *p;
            // TODO remove resolved pairs
        }
    }

    return a;

error:
    free(file_content);
    asm_free(a);
    return NULL;
}


static void asm_to_file(asm_t* a, char* path)
{
    FILE* f = fopen(path, "wb");
    ERR_IF(!f, ERR_FILE);
    fwrite(&a->code_size, 1, sizeof(a->code_size), f);
    fwrite(a->code, sizeof(*a->code), a->code_size, f);
    fclose(f);
    return;

error:
    if (f) fclose(f);
    return;
}

static bool  flag_help   = 0;
static char* output_file = "a.out";

static flag_t flags[] = {
    {
        .short_identifier = 'h',
        .long_identifier  = "help",
        .description      = "show this page and exit",
        .target           = &flag_help,
        .type             = FLAG_BOOL,
    },
    {
        .short_identifier = 'o',
        .long_identifier  = "output",
        .description      = "output file",
        .target           = &output_file,
        .type             = FLAG_STR,
    },
};

static uint32_t flags_len = sizeof(flags) / sizeof(*flags);

int main(int argc, char** argv)
{
    // flag stuff
    {
        int e = flag_parse(argc, argv, flags, flags_len, &argc, &argv);
        if (e) {
            fprintf(stderr, "Error in flag parsing\n");
            fprintf(stderr, "\t%s at '%s'\n", flag_error_format(e), *flag_error_position());
            exit(1);
        }
        if (flag_help) {
            flag_print_usage(stdout, "tvm a tiny vm", flags, flags_len);
            exit(0);
        }
    }

    asm_t* a = asm_from_file(argv[1]);

    if (err_get() != ERR_OK) {
        err_format(stderr);
        exit(1);
    }

    asm_to_file(a, output_file);

    if (err_get() != ERR_OK) {
        err_format(stderr);
        exit(1);
    }

    asm_free(a);

    return 0;
}
