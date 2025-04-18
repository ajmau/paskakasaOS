#ifndef SPINLOCK_H
#define SPINLOCK_H
#include <stdbool.h>
typedef struct {
    bool locked;
} spinlock_t;

void acquire(spinlock_t*);
void release(spinlock_t*);
#endif