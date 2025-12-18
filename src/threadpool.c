#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "threadpool.h"

typedef struct tpool_work {
    thread_func func;
    void *arg;
} tpool_work_t;

struct tpool {
    tpool_work_t *q;
    size_t qSize;
    int front;
    int back;
    int noJobs;

    pthread_mutex_t work_mutex;
    pthread_cond_t work_cond;
    pthread_cond_t working_cond;

    size_t thread_cnt;
    size_t working_cnt;

    bool stop;
};

int getJobCount(tpool_t *tpool) {
    return tpool->noJobs;
}

/*
static tpool_work_t *tpool_work_create(thread_func func, void *arg) {
    tpool_work_t *work = malloc(sizeof(tpool_work_t));

    if (work == NULL)
        return NULL;

    work->func = func;
    work->arg = arg;

    return work;
}

static void tpool_work_destroy(tpool_work_t *work) {
    if (work == NULL)
        return;

    free(work);
}
*/

static void tpool_work_enqueue(tpool_t *tpool, thread_func func, void *arg) {
    if (tpool == NULL || func == NULL)
        return;

    while (tpool->noJobs == tpool->qSize-1)
        pthread_cond_wait(&(tpool->working_cond), &(tpool->work_mutex));

    tpool->noJobs++;
    tpool->back++;
    tpool->back %= tpool->qSize;

    tpool->q[tpool->back] = (tpool_work_t){ func, arg };

    /*
    if (tpool->front == NULL) {
        tpool->front = work;
        tpool->back = work;
    }
    else {
        tpool->back->next = work;
        tpool->back = work;
    }
    */
}

static tpool_work_t *tpool_work_dequeue(tpool_t *tpool) {
    if (tpool == NULL || tpool->noJobs == 0)
        return NULL;
    
    tpool_work_t *work = &(tpool->q[tpool->front]);
    tpool->front++;
    tpool->front %= tpool->qSize;
    tpool->noJobs--;

    /*
    if (work->next == NULL) {
        tpool->front = NULL;
        tpool->back = NULL;
    }
    else {
        tpool->front = work->next;
    }
    */
    
    return work;
}

static void *tpool_worker(void *args) {
    tpool_t *tpool = args;

    while (1) {
        // Wait for available job
        pthread_mutex_lock(&(tpool->work_mutex));

        while (tpool->noJobs == 0 && !tpool->stop) {
            pthread_cond_wait(&(tpool->work_cond), &(tpool->work_mutex));
        }

        if (tpool->stop)
            break;

        // Take job from queue
        tpool_work_t *work = tpool_work_dequeue(tpool);

        tpool->working_cnt++;
        pthread_mutex_unlock(&(tpool->work_mutex));

        // Do job
        if (work != NULL) {
            thread_func func = work->func;
            void *arg = work->arg;
    
            func(arg);
    
            // tpool_work_destroy(work);
        }

        // Signal completed job
        pthread_mutex_lock(&(tpool->work_mutex));
        tpool->working_cnt--;
        pthread_cond_broadcast(&(tpool->working_cond));
        pthread_mutex_unlock(&(tpool->work_mutex));
    }

    tpool->thread_cnt--;
    pthread_cond_broadcast(&(tpool->working_cond));
    pthread_mutex_unlock(&(tpool->work_mutex));
    return NULL;
}

tpool_t *tpool_create(size_t noThreads, size_t qSize) {
    if (noThreads <= 0)
        return NULL;

    tpool_t *tpool = calloc(1, sizeof(tpool_t));

    if (tpool == NULL)
        return NULL;

    tpool->qSize = qSize;
    tpool->q = malloc(sizeof(tpool_work_t) * qSize);
    tpool->back = -1;

    pthread_mutex_init(&(tpool->work_mutex), NULL);
    pthread_cond_init(&(tpool->work_cond), NULL);
    pthread_cond_init(&(tpool->working_cond), NULL);

    tpool->thread_cnt = noThreads;

    for (size_t i = 0; i < noThreads; i++) {
        pthread_t thread;
        pthread_create(&thread, NULL, tpool_worker, tpool);
        pthread_detach(thread);
    }

    return tpool;
}

void tpool_destroy(tpool_t *tpool) {
    if (tpool == NULL)
        return;

    pthread_mutex_lock(&(tpool->work_mutex));

    while (tpool->noJobs != 0) {
        tpool_work_dequeue(tpool);
    }

    /*
    tpool_work_t *next = tpool->front;
    while (next != NULL) {
        next = tpool_work_destroy(next);
    }
    tpool->front = NULL;
    */

    tpool->stop = true;
    pthread_cond_broadcast(&(tpool->work_cond));

    pthread_mutex_unlock(&(tpool->work_mutex));

    tpool_wait(tpool);

    free(tpool->q);

    pthread_mutex_destroy(&(tpool->work_mutex));
    pthread_cond_destroy(&(tpool->work_cond));
    pthread_cond_destroy(&(tpool->working_cond));

    free(tpool);
}

// ret: 0 if sucessfully added work, 1 otherwise
int tpool_addWork(tpool_t *tpool, thread_func func, void *arg) {
    if (tpool == NULL)
        return 1;

    /*
    tpool_work_t *work = tpool_work_create(func, arg);
    if (work == NULL)
        return 1;
    */

    pthread_mutex_lock(&(tpool->work_mutex));

    tpool_work_enqueue(tpool, func, arg);

    pthread_cond_signal(&(tpool->work_cond));

    pthread_mutex_unlock(&(tpool->work_mutex));

    return 0;
}

void tpool_wait(tpool_t *tpool) {
    if (tpool == NULL)
        return;

    pthread_mutex_lock(&(tpool->work_mutex));

    while (tpool->noJobs != 0 || (!tpool->stop && tpool->working_cnt != 0) || (tpool->stop && tpool->thread_cnt > 0)) {
        pthread_cond_wait(&(tpool->working_cond), &(tpool->work_mutex));
    }

    pthread_mutex_unlock(&(tpool->work_mutex));
}