#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../memory_allocator/heap_memory.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <errno.h>

// task to be executed
typedef struct {
    void (*function)(void *);
    void *arg;                 
} job;

// circular job queue
typedef struct job_queue {
    job *jobs;          
    int max_cap;        
    int loading;         
    int head;            
    int tail;            
    pthread_mutex_t lock;
    sem_t not_empty;    
    sem_t not_full;     
    bool shutdown;     
} job_queue;

// Thread pool 
typedef struct {
    pthread_t *threads;  
    int thread_count;    
    int stop;            
    job_queue queue;     
} thread_pool;

/**
 * Initialize a job queue
 * @param queue Pointer to job queue
 * @param capacity Maximum number of jobs in queue
 * @return 0 on success, -1 on failure
 */
int job_queue_init(job_queue *queue, int capacity);

/**
 * Enqueue a job into the job queue
 * @param queue Pointer to job queue
 * @param job Job to be added
 * @return 0 on success, -1 on failure
 */
int job_queue_enqueue(job_queue *queue, job job_item);

/**
 * Dequeue a job from the job queue
 * @param queue Pointer to job queue
 * @param job Pointer to store dequeued job
 * @return 0 on success, -1 on failure
 */
int job_queue_dequeue(job_queue *queue, job *job_item);

/**
 * free and  destroy job queue
 * @param queue Pointer to job queue
 */
void job_queue_destroy(job_queue *queue);

/**
 * Initialize thread pool
 * @param pool Pointer to thread pool
 * @param thread_count Number of worker threads
 * @param queue_size Job queue size
 * @return 0 on success, -1 on failure
 */
int init_pool(thread_pool *pool, int thread_count, int queue_size);

/**
 * Add a job to thread pool
 * @param pool Pointer to thread pool
 * @param function Function to execute
 * @param arg Function argument
 * @return 0 on success, -1 on failure
 */
int add_job(thread_pool *pool, void (*function)(void *), void *arg);

/**
 * Shutdown and free thread pool resources
 * @param pool Pointer to thread pool
 */
void free_pool(thread_pool *pool);

#endif 