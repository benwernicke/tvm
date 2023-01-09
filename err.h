#ifndef ERR_H
#define ERR_H

typedef enum {
    ERR_OK = 0,
    ERR_BAD_MALLOC,
    ERR_BAD_FILE,
} err_t;

void err_set(err_t e);
const char* err_format(err_t e);
err_t err_get(void);

#define ERR(e)                  \
    do {                        \
        err_set(e) ;            \
        goto err_handling;      \
    } while (0)

#define ERR_IF(b, e)            \
    do {                        \
        if (b) {                \
            ERR(e);             \
        }                       \
    } while (0)

#define ERR_FORWARD()               \
    do {                            \
        if (err_get() != ERR_OK) {  \
            goto err_handling;      \
        }                           \
    } while (0)


#endif
