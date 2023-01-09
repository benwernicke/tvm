#include "err.h"

static err_t err = ERR_OK;

static const char* err_str[] = {
    [ERR_OK] = "ERR_OK",
    [ERR_BAD_MALLOC] = "ERR_BAD_MALLOC",
    [ERR_BAD_FILE] = "ERR_BAD_FILE",
};

void err_set(err_t e)
{
    err = e;
}

const char* err_format(err_t e)
{
    return err_str[e];
}

err_t err_get(void)
{
    return err;
}
