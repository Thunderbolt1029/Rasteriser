#ifndef THREADPOOL_H
#define THREADPOOL_H

struct tpool;
typedef struct tpool tpool_t;

typedef void (*thread_func)(void *arg);

tpool_t *tpool_create(size_t noThreads);
void tpool_destroy(tpool_t *tpool);

int tpool_addWork(tpool_t *tpool, thread_func func, void *arg);
void tpool_wait(tpool_t *tpool);

#endif