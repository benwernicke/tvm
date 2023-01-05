#ifndef ERROR_H
#define ERROR_H

#define ERROR(t)                                                               \
    do {                                                                       \
        goto t;                                                                \
    } while (0)

#define ERROR_IF(b, t)                                                         \
    do {                                                                       \
        if (b)                                                                 \
            ERROR(t);                                                          \
    } while (0)

#endif
