#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "err.h"

static err_t error;

const char* err_str_map[] = {
    [ERR_OK]         = "ERR_OK",
    [ERR_BAD_MALLOC] = "ERR_BAD_MALLOC",
    [ERR_ASSEMBLER]  = "ERR_ASSEMBLER",
    [ERR_FILE]       = "ERR_FILE",
    [ERR_VM]         = "ERR_VM",
};

const uint64_t err_maps_size = 
    sizeof(err_str_map) / sizeof(*err_str_map);

err_t err_get(void)
{
    return error;
}

void err_set(err_t e)
{
    error = e;
}

void err_format(FILE* stream)
{
    switch (error) {
    case ERR_OK: 
        fprintf(stream, "Error: no error reported\n");
        break;
    case ERR_BAD_MALLOC: 
        fprintf(stream, "Error: failed allocation:\n");
        fprintf(stream, "\t%s\n", strerror(errno));
        break;
    case ERR_ASSEMBLER: 
        fprintf(stream, "Error: assembler failed! Additional output above\n");
    break;
    case ERR_FILE: 
        fprintf(stream, "Error: failed file operation\n");
        fprintf(stream, "\t%s\n", strerror(errno));
    break;
    case ERR_VM: 
        fprintf(stream, "Error: vm failed! Additional output above");
    break;
    };
}
