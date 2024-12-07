#include "../include/thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void *worker_thread(void *arg);


int job_queue_init(job_queue *queue, int capacity) {
    if (capacity <= 0) {
        errno = EINVAL;
        return -1;
    }

    queue->jobs = (job *)valloc(sizeof(job) * capacity);
    if (!queue->jobs) {
        return -1;
    }

    queue->max_cap = capacity;
    queue->loading = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->shutdown = false;

    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        vfree(queue->jobs);
        return -1;
    }

    if (sem_init(&queue->not_empty, 0, 0) != 0) {
        pthread_mutex_destroy(&queue->lock);
        vfree(queue->jobs);
        return -1;
    }

    if (sem_init(&queue->not_full, 0, capacity) != 0) {
        sem_destroy(&queue->not_empty);
        pthread_mutex_destroy(&queue->lock);
        vfree(queue->jobs);
        return -1;
    }

    return 0;
}

int job_queue_enqueue(job_queue *queue, job job_item) {
    if (queue->shutdown) {
        errno = ESHUTDOWN;
        return -1;
    }

    sem_wait(&queue->not_full);
    
    if (pthread_mutex_lock(&queue->lock) != 0) {
        sem_post(&queue->not_full);
        return -1;
    }

    queue->jobs[queue->tail] = job_item;
    queue->tail = (queue->tail + 1) % queue->max_cap;
    queue->loading++;

    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->not_empty);

    return 0;
}

int job_queue_dequeue(job_queue *queue, job *job_item) {
    sem_wait(&queue->not_empty);
    
    if (pthread_mutex_lock(&queue->lock) != 0) {
        sem_post(&queue->not_empty);
        return -1;
    }

    if (queue->shutdown) {
        pthread_mutex_unlock(&queue->lock);
        return -1;
    }

    *job_item = queue->jobs[queue->head];
    queue->head = (queue->head + 1) % queue->max_cap;
    queue->loading--;

    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->not_full);

    return 0;
}

void job_queue_destroy(job_queue *queue) {
    queue->shutdown = true;
    sem_post(&queue->not_empty); 
    
    pthread_mutex_destroy(&queue->lock);
    sem_destroy(&queue->not_empty);
    sem_destroy(&queue->not_full);
    
    vfree(queue->jobs);
}

static void *worker_thread(void *arg) {
    thread_pool *pool = (thread_pool *)arg;
    job job_item;

    while (1) {
        if (job_queue_dequeue(&pool->queue, &job_item) == 0) {
            job_item.function(job_item.arg);
        } else {
            break;
        }
    }

    return NULL;
}

int init_pool(thread_pool *pool, int thread_count, int queue_size) {
    if (thread_count <= 0 || queue_size <= 0) {
        errno = EINVAL;
        return -1;
    }

    pool->threads = (pthread_t *)valloc(thread_count * sizeof(pthread_t));
    if (!pool->threads) {
        return -1;
    }

    pool->thread_count = thread_count;
    pool->stop = 0;

    if (job_queue_init(&pool->queue, queue_size) != 0) {
        vfree(pool->threads);
        return -1;
    }

    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            //cleanup
            pool->stop = 1;
            free_pool(pool);
            return -1;
        }
    }

    return 0;
}

int add_job(thread_pool *pool, void (*function)(void *), void *arg) {
    job job_item = {
        .function = function,
        .arg = arg
    };
    return job_queue_enqueue(&pool->queue, job_item);
}

void free_pool(thread_pool *pool) {
    job_queue_destroy(&pool->queue);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_cancel(pool->threads[i]);
        pthread_join(pool->threads[i], NULL);
    }

    vfree(pool->threads);
}