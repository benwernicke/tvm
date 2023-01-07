#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "flag.h"
#include "assembler.h"

static bool  flag_help   = 0;
static bool  flag_asm    = 0;
static char* flag_output = NULL;

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
        .description      = "assemble the given file into tvm byte code",
        .target           = &flag_asm,
        .type             = FLAG_BOOL,
    },
    {
        .short_identifier = 'o',
        .long_identifier  = "output",
        .description      = "use argument as output file",
        .target           = &flag_output,
        .type             = FLAG_STR,
    },
};

static uint32_t flags_len = sizeof(flags) / sizeof(*flags);


int main(int argc, char** argv)
{
    {
        int e = flag_parse(argc, argv, flags, flags_len, &argc, &argv);
        if(e) {
            fprintf(stderr, "Error: '%s' at '%s'\n", 
                    flag_error_format(e), 
                    *flag_error_position());
            exit(1);
        } else if (flag_help) {
            flag_print_usage(stdout, "Tiny VM <0x0>", flags, flags_len);
            exit(0);
        }
    }

    if (flag_asm) {
        int e = assemble(flag_output ? flag_output : "a.out", argv + 1, argc - 1);
        if (e) {
            fprintf(stderr, "Error: some error in assembler."
                    " More output should be above!\n");
        }
    }
    return 0;
}
