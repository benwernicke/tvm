#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/error.h"
#include "lib/flag.h"

#define GENERAL_USAGE "an assembler for the tvm"

typedef struct prog_t prog_t;
struct prog_t {
    instr_t* buf;
    uint32_t size;
    uint32_t cap;
};

static instr_t* parse_code(char* path)
{

}

static bool flag_help = 0;

flag_t flags[] = {
    {
        .short_identifier = 'h',
        .long_identifier = "help",
        .description = "show this page and exit",
        .target = &flag_help,
        .type = FLAG_BOOL,
    },
};
static const uint32_t flags_len = sizeof(flags) / sizeof(*flags);

int main(int argc, char** argv)
{
    // flag and arg handling
    {
        int e = flag_parse(argc, argv, flags, flags_len, NULL, NULL);
        ERROR_IF(e, error);

        if (flag_help) {
            flag_print_usage(stdout, GENERAL_USAGE, flags, flags_len);
            return 0;
        }
    }
    return 0;

error:
    fprintf(stderr, "some error occured\n");
    return 1;
}
