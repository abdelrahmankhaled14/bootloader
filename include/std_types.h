#ifndef STDTYPES_H
#define STDTYPES_H

#include <stdint.h>
#include <stddef.h>

typedef void(*Runnable_Cbg_t)(void *arg);
/* Boolean type */
typedef enum {
    FALSE = 0,
    TRUE  = 1
} bool_t;

/* Null pointer */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* Status return type */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR
} status_t;

typedef enum {
    DISABLE = 0,
    ENABLE  = 1
} state_t;
typedef void (*Callback_t)(void);

#endif /* STDTYPES_H */
