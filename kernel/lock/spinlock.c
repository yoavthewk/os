#include <kernel/lock/spinlock.h>

void acquire(spinlock_t* lock) {
    // returns the previous state of the lock; if it returns true, we've acquired it.
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        // yield?
    }
}

void release(spinlock_t* lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}
