#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    volatile bool locked;
} spinlock_t;

void acquire(spinlock_t* lock);
void release(spinlock_t* lock);
