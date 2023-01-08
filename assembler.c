#include "assembler.h"
#include "def.h"
#include "label_map.h"

#include <stddef.h>
#include <stdio.h>
#include <ctype.h>

typedef enum {
    ASM_ERR_OK = 0,
    ASM_ERR_BAD_MALLOC,
    ASM_ERR_FILE,
} asm_err_t;

static asm_err_t err = 0;

#define ERROR_IF(b, v)              \
        do {                        \
            if (b) {                \
                err = v;            \
                goto handle_err;    \
            }                       \
        } while(0)

#define ERROR_FORWARD()             \
        do{                         \
            if (err) {              \
                goto handle_err;    \
            }                       \
        } while (0)

typedef struct assemble_t assemble_t;
struct assemble_t {
    // general stuff
    uint64_t* code; 
    uint64_t  code_size;
    uint64_t  code_cap;
    char*     data;
    uint64_t  data_size;
    uint64_t  data_cap;

    // linking stuff
    label_map_t* label_map;
};

static char* trim_and_remove_comment(char* s)
{
    for(; isspace(*s); ++s) {  }
    char* e = s;
    for(; *e  && *e != '#'; ++e) {  }
    *e-- = 0;
    for(; isspace(*e); --e) {  }
    *(++e) = 0;
    return s;
}

static void first_word(char* s, char** start, char** end)
{
    for(; isspace(*s); ++s) {  }
    if(!*s) {
        *start = NULL;
        return;
    }
    *start = s;
    for(; *s && !isspace(*s); ++s);
    *end = s;
} 

static void assemble_free(assemble_t* a)
{
    if(a) {
        free(a->code);
        free(a->data);
        label_map_free(a->label_map);
        free(a);
    }
}

static assemble_t* assemble_create(void)
{
    assemble_t* a = calloc(1, sizeof(*a));
    ERROR_IF(!a, ASM_ERR_BAD_MALLOC);

    a->label_map = label_map_create();
    ERROR_IF(!a->label_map, ASM_ERR_BAD_MALLOC);

    return a;

handle_err:
    assemble_free(a);
    return NULL;
}

static assemble_t* assemble_single_file(char* path)
{
    FILE*       fh   = NULL;
    assemble_t* ass  = NULL;
    char*       line = NULL;

    ass = assemble_create();
    ERROR_FORWARD();
    fh = fopen(path, "r");
    ERROR_IF(!fh, ASM_ERR_FILE);

    uint64_t line_cap = 0;
    uint64_t rip = 0;

    for (; getline(&line, &line_cap, fh) > 0; ++rip ) {
        ERROR_IF(!line, ASM_ERR_BAD_MALLOC);
        char* s = trim_and_remove_comment(line);

        if(!*s) {
            continue;
        }

        char* e;
        first_word(s, &s, &e);
        if(*(e - 1) == ':') {
            *(--e) = 0;
            ERROR_IF(label_map_insert(ass->label_map, s, rip), 
                    ASM_ERR_BAD_MALLOC);
        } else {
            *e  = 0;
            if(strcmp(s, "@data")) {
            } else if (strcmp(s, "lea")  == 0) {
            } else if (strcmp(s, "mov")  == 0) {
            } else if (strcmp(s, "exit") == 0) {
            } else if (strcmp(s, "ret")  == 0) {
            } else if (strcmp(s, "cmov") == 0) {
            } else if (strcmp(s, "jmp")  == 0) {
            } else if (strcmp(s, "cjmp") == 0) {
            } else if (strcmp(s, "push") == 0) {
            } else if (strcmp(s, "pop")  == 0) {
            } else if (strcmp(s, "stor") == 0) {
            } else if (strcmp(s, "load") == 0) {
            } else if (strcmp(s, "add")  == 0) {
            } else if (strcmp(s, "sub")  == 0) {
            } else if (strcmp(s, "mul")  == 0) {
            } else if (strcmp(s, "div")  == 0) {
            } else if (strcmp(s, "mod")  == 0) {
            } else if (strcmp(s, "and")  == 0) {
            } else if (strcmp(s, "or")   == 0) {
            } else if (strcmp(s, "xor")  == 0) {
            } else if (strcmp(s, "comp") == 0) {
            } else if (strcmp(s, "sl")   == 0) {
            } else if (strcmp(s, "sr")   == 0) {
            } else if (strcmp(s, "call") == 0) {
            } else if (strcmp(s, "eq")   == 0) {
            } else if (strcmp(s, "neq")  == 0) {
            } else if (strcmp(s, "gt")   == 0) {
            } else if (strcmp(s, "geq")  == 0) {
            } else if (strcmp(s, "lt")   == 0) {
            } else if (strcmp(s, "leq")  == 0) {
            } else if (strcmp(s, "not")  == 0) {
            } else if (strcmp(s, "syscall") == 0) {
            } else {
            }
        }   
    }

    return ass;

handle_err:
    assemble_free(ass);
    if (fh) fclose(fh);
    free(line);
    
    return NULL;
}

static assemble_t* link(assemble_t** assemblies, uint32_t assemblies_len)
{
    // TODO
    return NULL;
}

static void to_file(char* target, assemble_t* ass)
{
    // TODO
    return;
}

int assemble(char* target, char** src, uint32_t src_len)
{
    uint32_t i = 0;
    assemble_t* ass[src_len];
    for(; i < src_len; ++i) {
        ass[i] = assemble_single_file(src[i]);
        if (err) {
            fprintf(stderr, "Some error occured in assembler!\n");
            exit(1);
        }
    }
    assemble_t* master = link(ass, src_len);
    to_file(target, master);
    return 0;
}
