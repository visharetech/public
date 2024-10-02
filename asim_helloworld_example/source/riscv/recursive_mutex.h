#ifndef _RECURSIVE_MUTEX_H_
#define _RECURSIVE_MUTEX_H_

#define RECURSIVE_MUTEX_INITIALIZER {.mutex = PTHREAD_MUTEX_INITIALIZER, .owner=0, .count = 0};

typedef struct {
    pthread_mutex_t mutex;
    pthread_t owner;
    int count;
} recursive_mutex_t;

static inline void recursive_mutex_init(recursive_mutex_t* lock) {
    pthread_mutex_init(&lock->mutex, NULL);
    lock->owner = 0;
    lock->count = 0;
}

static inline void recursive_mutex_lock(recursive_mutex_t* lock) {
    pthread_t current_thread = pthread_self();

    if (current_thread == lock->owner) {
        lock->count++;
        return;
    }

    pthread_mutex_lock(&lock->mutex);
    lock->owner = current_thread;
    lock->count = 1;
}

static inline void recursive_mutex_unlock(recursive_mutex_t* lock) {
    if (lock->count > 1) {
        lock->count--;
        return;
    }

    lock->owner = 0;
    lock->count = 0;
    pthread_mutex_unlock(&lock->mutex);
}

#endif

