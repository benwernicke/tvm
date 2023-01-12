#ifndef LABEL_BUF_H
#define LABEL_BUF_H

#include <stdlib.h>
#include <stdint.h>

typedef struct label_buf_pair_t label_buf_pair_t;
struct label_buf_pair_t {
    char* label;
    uint64_t* pos;
};

typedef struct label_buf_t label_buf_t;
struct label_buf_t {
    uint64_t size;
    uint64_t cap;
    label_buf_pair_t* buf;
};

label_buf_t* label_buf_create(void);
label_buf_pair_t* label_buf_more(label_buf_t* b);
label_buf_pair_t* label_buf_begin(label_buf_t* b);
label_buf_pair_t* label_buf_end(label_buf_t* b);
void label_buf_free(label_buf_t* b);

#endif // LABEL_BUF_H
