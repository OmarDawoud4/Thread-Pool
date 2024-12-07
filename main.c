#include <stdio.h>
#include "include/thread_pool.h"
#include <unistd.h>

void job_function(void *arg) {
    int num = *(int *)arg;
    printf("Job %d started\n", num);
    sleep(2);  // work
    printf("Job %d completed\n", num);
}

void another_job_function(void *arg) {
    char *message = (char *)arg;
    printf("Message: %s\n", message);
    sleep(1); 
}

int main() {
    thread_pool pool;

    // 4 threads and a queue size of 10
    init_pool(&pool, 4, 10);

    for (int i = 0; i < 10; i++) {
        int *job_id = (int *)valloc(sizeof(int));
        *job_id = i;
        add_job(&pool, job_function, job_id); 
    }

    for (int i = 0; i < 5; i++) {
        char *message = (char *)valloc(50 * sizeof(char));
        snprintf(message, 50, "Job number %d with custom message", i);
        add_job(&pool, another_job_function, message); 
    }


    sleep(12);  

    free_pool(&pool);

 
}
