#include "lib/beaver.h"

#define FLAGS "-g -Wall -Werror -Og"

module_t modules[] = {
    { .name = "assembler", .src = "assembler.c" },
    { .name = "assembler", .src = "lib/flag.c" },
};

uint32_t modules_len = sizeof(modules) / sizeof(*modules);

char* assembler[] = { "assembler", NULL };


int main(int argc, char** argv)
{
    auto_update(argv);
    if (argc == 1) {
        compile(assembler, FLAGS);
    } else if (strcmp(argv[1], "clean") == 0) {
        rm("$(find build -type f)");
        rm("out");
    } else {
        fprintf(stderr, "\033[31mError:\033[39m unknown option: '%s'\n", argv[1]);
        exit(1);
    }
    return 0;
}
