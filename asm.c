#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "def.h"
#include "err.h"
#include "flag.h"
#include "map.h"
#include "label_buf.h"

typedef struct asm_t asm_t;
struct asm_t {
    uint64_t*    code;
    uint64_t     code_size;
    uint64_t     code_cap;
    map_t*       label_map;
    label_buf_t* label_buf;
    char*        data;
    uint64_t     data_size;
    uint64_t     data_cap;
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

static bool token_is_char_literal(char* s, char* e)
{
    if (e - s == 3) {
        if (s[0] != '\'' || s[2] != '\'') {
            return 0;
        }
        return 1;
    }

    if (e - s == 4) {
        if (s[0] != '\'' || s[3] != '\'' || s[1] != '\\') {
            return 0;
        }
        return 1;
    }

    return 0;
}

static bool token_is_literal(char* s, char* e)
{
    if (token_is_char_literal(s, e)) {
        return 1;
    }

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

static bool token_is_type_identifier(char* s, char* e)
{
    if (strncmp(s, "char", 4) == 0) {
        if (s[4] == '[') {
            return *(e - 1) == ']';
        } else if (e - s == 4) {
            return 1;
        } else {
            ERR(ERR_ASSEMBLER);
        }
    } else if(strncmp(s, "u8", 2) == 0) {
        if (s[2] == '[') {
            return *(e - 1) == ']';
        } else if (e - s == 2) {
            return 1;
        } else {
            ERR(ERR_ASSEMBLER);
        }
    } else if(strncmp(s, "u16", 3) == 0) {
        if (s[3] == '[') {
            return *(e - 1) == ']';
        } else if (e - s == 3) {
            return 1;
        } else {
            ERR(ERR_ASSEMBLER);
        }
    } else if(strncmp(s, "u32", 3) == 0) {
        if (s[3] == '[') {
            return *(e - 1) == ']';
        } else if (e - s == 3) {
            return 1;
        } else {
            ERR(ERR_ASSEMBLER);
        }
    } else if(strncmp(s, "u64", 3) == 0) {
        if (s[3] == '[') {
            return *(e - 1) == ']';
        } else if (e - s == 3) {
            return 1;
        } else {
            ERR(ERR_ASSEMBLER);
        }
    }

error:
    return 0;
}

static uint64_t token_get_type_identifier_size(char* s, char* e)
{
    uint64_t r = 0;

    if (strncmp(s, "char", 4) == 0) {
        if (s[4] == '[') {
            s += 5;
            e -= 1;
            *e = 0;
            r = 1 * atol(s);
        } else {
            r = 1;
        }
        goto end;
    } else if(strncmp(s, "u8", 2) == 0) {
        if (s[2] == '[') {
            s += 3;
            e -= 1;
            *e = 0;
            r = 1 * atol(s);
        } else {
            r = 1;
        }
        goto end;
    } else if(strncmp(s, "u16", 3) == 0) {
        if (s[3] == '[') {
            s += 4;
            e -= 1;
            *e = 0;
            r = 2 * atol(s);
        } else {
            r = 1;
        }
        goto end;
    } else if(strncmp(s, "u32", 3) == 0) {
        if (s[3] == '[') {
            s += 4;
            e -= 1;
            *e = 0;
            r = 4 * atol(s);
        } else {
            r = 1;
        }
        goto end;
    } else if(strncmp(s, "u64", 3) == 0) {
        if (s[3] == '[') {
            s += 4;
            e -= 1;
            *e = 0;
            r = 8 * atol(s);
        } else {
            r = 1;
        }
        goto end;
    }

end:
    return r;

}

static char* asm_data_more(asm_t* a, uint64_t size)
{
    if (a->data_size + size >= a->data_cap) {
        a->data_cap <<= 1;
        a->data_cap += size;
        a->data = realloc(a->data, a->data_cap);
        ERR_IF(!a->data, ERR_BAD_MALLOC);
    }
    char* r = &a->data[a->data_size];
    a->data_size += size;
    return r;

error:
    return NULL;
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
    if (token_is_char_literal(s, e)) {
        if (e - s == 3) {
            return s[1];
        }
        switch (s[2]) {
        case 'n': return '\n';
        case '0': return '\0';
        }
    }

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
            uintptr_t iter = map_begin(a->label_map);
            for (; iter != map_end(a->label_map); iter = map_next(a->label_map, iter) ) {
                free(*map_iter_key(a->label_map, iter));
            }
            map_free(a->label_map);
        }
        {
            label_buf_pair_t* iter = label_buf_begin(a->label_buf);
            label_buf_pair_t* end  = label_buf_end(a->label_buf);
            for (; iter != end; ++iter) {
                free(iter->label);
            }
            label_buf_free(a->label_buf);
        }
        free(a->data);
        free(a->code);
        free(a);
    }
}

static void asm_data_init(uint64_t* line_number, char* data, uint64_t data_size, char* s, char** start, char** end)
{
    s = trim_comments(line_number, s);
    if (!*s) {
        fprintf(stderr, "Error: expected storage initializer and got nothing\n");
        fprintf(stderr, "\tat line %lu\n", *line_number);
        ERR(ERR_ASSEMBLER);
    }

    if (*s == '"') {
        s += 1;
        *start = s;
        for(; *s && *s != '\n' && *s != '"'; ++s) { }
        *end = s;
        if (!*s || *s == '\n') {
            fprintf(stderr, "Error: no matching '\"'\n");
            fprintf(stderr, "\tat line %lu\n", *line_number);
            ERR(ERR_ASSEMBLER);
        }
        if (end - start > data_size) {
            fprintf(stderr, "Error: storage is not big enough\n");
            fprintf(stderr, "\tat line %lu\n", *line_number);
            ERR(ERR_ASSEMBLER);
        }
        char* s = *start;
        for (; s != *end; ++s) {
            if (*s == '\\') {
                switch(*(++s)) {
                case 'n': 
                    *data++ = '\n';
                    break;
                case '0':
                    *data++ = '\0';
                    break;
                }
            } else {
                *data++ = *s;
            }
        }
        *end += 1;
    } else {
        *start = s;
        for (; *s && !isspace(*s); ++s) {  }
        *end = s;
        char t = **end;
        **end = 0;
        if (strcmp(*start, "default") == 0) {
            memset(data, 0, data_size);
        }
        **end = t;
    }

    return;

error:
    return;
}

static void resolve_labels(asm_t* a)
{
    label_buf_pair_t* iter = label_buf_begin(a->label_buf);
    label_buf_pair_t* end  = label_buf_end(a->label_buf);
    for (; iter != end; ++iter) {
        uint64_t* p = map_get(a->label_map, iter->label);
        if (!p) {
            fprintf(stderr, "Error: unknown label '%s'\n", iter->label);
            // TODO line number
            ERR(ERR_ASSEMBLER);
        }
        a->code[iter->pos] = *p;
    }

    return;

error:
    return;
}

static asm_t* asm_from_file(char* path)
{
    char*    file_content = NULL;
    uint64_t line_number  = 1;
    asm_t* a = calloc(1, sizeof(*a));
    ERR_IF(!a, ERR_BAD_MALLOC);
    a->label_map = map_create();
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
            uint64_t labels[3] = { 0 };
            memset(labels, 0xFF, sizeof(labels));
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
                    labels[i] = a->label_buf->size - 1;

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
                if (labels[i] != 0xFFFFFFFFFFFFFFFFUL) {
                    a->label_buf->buf[labels[i]].pos = a->code_size - 1;
                }
            }
        } else if (token_is_label(s, e)) {
            bool is_mem_label = 0;
            char* l = NULL;

            e  -= 1;
            *e  = 0;
            ip -= 1; // get_token advances ip but label is not an instruction

            // file private label
            if(*s == ':') {
                is_mem_label = s[1] == '@';
                l = malloc(strlen(path) + (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                *l = 0;
                strcat(l, path);
                last_label = l;

            // function private label
            } else if (*s == '.') {
                is_mem_label = s[1] == '@';
                l = calloc(1, strlen(last_label) + (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                *l = 0;
                strcat(l, last_label);

            // file public label
            } else {
                is_mem_label = s[0] == '@';
                l = calloc(1, (e - s) + 1);
                ERR_IF(!l, ERR_BAD_MALLOC);
                last_label = l;
            }
            strcat(l, s);
            e += 1;
            if (is_mem_label) {
                get_token(&line_number, &ip, e, &s, &e);
                ip -= 1; // we dont need ip
                if (!token_is_type_identifier(s, e)) {
                    fprintf(stderr, "Error: expected type identifier after storage label '%s'\n", l);
                    fprintf(stderr, "\tat line %lu\n", line_number);
                    ERR(ERR_ASSEMBLER);
                }
                uint64_t mem_size = token_get_type_identifier_size(s, e);

                int err = map_insert(a->label_map, l, a->data_size);
                char* d = asm_data_more(a, mem_size);
                ERR_IF(!d || err, ERR_BAD_MALLOC);
                asm_data_init(&line_number, d, mem_size, e, &s, &e);
            } else {
                int err = map_insert(a->label_map, l, ip - 1);
                ERR_IF(err, ERR_BAD_MALLOC);
            }
        } else {
            *e = 0;
            fprintf(stderr, "Error: '%s' is not a valid instruction nor label definition\n", s);
            fprintf(stderr, "\tat line %lu\n", line_number);
            ERR(ERR_ASSEMBLER);
        }
        get_token(&line_number, &ip, e, &s, &e);
    } while (s && *s);

    free(file_content);

    return a;

error:
    free(file_content);
    asm_free(a);
    return NULL;
}

void asm_link(asm_t* dest, asm_t* src)
{
    // join label buf
    {
        label_buf_pair_t* i = label_buf_begin(src->label_buf);
        label_buf_pair_t* e = label_buf_end(src->label_buf);
        for (; i != e; ++i) {
            label_buf_pair_t* p = label_buf_more(dest->label_buf);
            ERR_IF(!p, ERR_BAD_MALLOC);
            *p = *i;
            p->pos += dest->code_size;
        }
        label_buf_free(src->label_buf);
    }

    // join label maps
    {
        uintptr_t i = map_begin(src->label_map);
        uintptr_t e = map_end(src->label_map);

        for (; i != e; i = map_next(src->label_map, i)) {
            char* key    = *map_iter_key(src->label_map, i);
            uint64_t val = *map_iter_value(src->label_map, i);

            if (map_contains(dest->label_map, key)) {
                fprintf(stderr, "Error in Linker: Multiple definitions of label: '%s'\n", key);
                ERR(ERR_ASSEMBLER);
            }

            if (strchr(key, '@')) {
                val += dest->data_size;
            } else {
                val += dest->code_size;
            }

            int e = map_insert(dest->label_map, key, val);
            ERR_IF(e, ERR_BAD_MALLOC);
        }
        map_free(src->label_map);
    }

    // join code segment
    {
        if (src->code) {
            dest->code_cap += src->code_size;
            dest->code = realloc(dest->code, dest->code_cap * sizeof(*dest->code));
            ERR_IF(!dest->code, ERR_BAD_MALLOC);
            memcpy(dest->code + dest->code_size, src->code, src->code_size * sizeof(*src->code));
            dest->code_size += src->code_size;
            free(src->code);
        }
    }

    // join data segment
    {
        if (src->data) {
            dest->data_cap += src->data_size;
            dest->data = realloc(dest->data, dest->data_cap * sizeof(*dest->data));
            ERR_IF(!dest->data, ERR_BAD_MALLOC);
            memcpy(dest->data + dest->data_size, src->data, src->data_size * sizeof(*src->data));
            dest->data_size += src->data_size;
            free(src->data);
        }
    }

    free(src);

error:
    return;
}

static asm_t* asm_default(void)
{
    asm_t* a = calloc(1, sizeof(*a));
    ERR_IF(!a, ERR_BAD_MALLOC);
    a->label_map = map_create();
    a->label_buf = label_buf_create();
    ERR_IF(!a->label_map || !a->label_buf, ERR_BAD_MALLOC);

    uint64_t* n = asm_more(a);
    ERR_IF(!n, ERR_BAD_MALLOC);

    *n = instr_bin_map[INSTR_JMP] | arg_bin_map[ARG_L];

    n = asm_more(a);
    ERR_IF(!n, ERR_BAD_MALLOC);

    label_buf_pair_t* p = label_buf_more(a->label_buf);
    ERR_IF(!p, ERR_BAD_MALLOC);
    p->label = strdup("main");
    p->pos   = 1;

    return a;

error:
    if (a) { 
        free(a->label_map);
        free(a->label_buf);
        free(a->code);
        free(a->data);
        free(a);
    }
    return NULL;
}

static void asm_to_file(asm_t* a, char* path)
{
    FILE* f = fopen(path, "wb");
    ERR_IF(!f, ERR_FILE);
    fwrite(&a->code_size, 1, sizeof(a->code_size), f);
    fwrite(&a->data_size, 1, sizeof(a->data_size), f);
    fwrite(a->code, sizeof(*a->code), a->code_size, f);

    if (a->data_size) {
        fwrite(a->data, sizeof(*a->data), a->data_size, f);
    }

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

    asm_t* main_asm = asm_default();

    // iterate over all source files
    {
        char** iter = argv + 1;
        char** end  = argv + argc;
        for (; iter != end; ++iter) {
            asm_t* a = asm_from_file(*iter);
            ERR_FORWARD();
            asm_link(main_asm, a);
            ERR_FORWARD();
        }
    }

    resolve_labels(main_asm);

    asm_to_file(main_asm, output_file);
    ERR_FORWARD();

    asm_free(main_asm);

    return 0;

error:
    err_format(stderr);
    return 1;
}
