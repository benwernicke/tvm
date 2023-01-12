#include "label_buf.h"

label_buf_t* label_buf_create(void)
{
    return calloc(1, sizeof(label_buf_t));
}

label_buf_pair_t* label_buf_more(label_buf_t* b)
{
    if (b->size >= b->cap) {
        b->cap += 1;
        b->cap <<= 1;
        b->buf = realloc(b->buf, b->cap * sizeof(*b->buf));
        if (!b->buf) {
            return NULL;
        }
    }
    return &b->buf[b->size++];
}

label_buf_pair_t* label_buf_begin(label_buf_t* b)
{
    return b->buf;
}

label_buf_pair_t* label_buf_end(label_buf_t* b)
{
    return b->buf + b->size;
}

void label_buf_free(label_buf_t* b)
{
    if (b) {
        free(b->buf);
        free(b);
    }
}
