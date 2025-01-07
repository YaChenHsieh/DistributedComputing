#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 7

// Init the mutex and cond
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Define the required order -> shared data
int order[NUM_THREADS] = {1, 4, 2, 3, 7, 6, 5};
int current = 0; //useing current as cond

// Array of the threads that hold thread numbers
int thread_num[NUM_THREADS];

// Thread func
// void* arg: A pointer to an integer representing the thread number.
// int thread_num = *(int*)arg: Dereferences the arg pointer -> get the actual thread number.
void* thread_func(void* arg){ 
    int thread_num = *(int*)arg; 

    pthread_mutex_lock(&mutex);

    // Wait until it's this thread's turn
    while(order[current]!=thread_num){
        pthread_cond_wait(&cond, &mutex);
    }

    // It's this thread's turn
    printf("Hello from thread %d\n", thread_num);
    current ++; //increase to the next order
    pthread_cond_broadcast(&cond);

    pthread_mutex_unlock(&mutex);

    // free the memory
    free(arg);
    return NULL;

}

// Main func
int main(){
    // init threads
    pthread_t threads[NUM_THREADS];

    //iterate all the threads and assing mem 
    for(int i=1; i<=NUM_THREADS; ++i){
        //assing mem for the num pointer

        int* num = malloc(sizeof(int));

        // Checks if malloc succeeded. If it failed, prints an error message and exits the program.
        if(num == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        
        // Stores the current loop index i ( 1~7 ) in the allocated memory(pointer addr)
        *num = i; 

        // 
        // &thread[i-1] -> bc thread index still count from thread[0] && num from 1-7
        if(pthread_create(&threads[i-1], NULL, thread_func, num) !=0 ){ 
            perror("pthread_create");
            exit(EXIT_FAILURE); 
        } 
    }

    // Join all threads
    for(int i=0; i<NUM_THREADS;++i){
        pthread_join(threads[i], NULL); //NOT using &thread bc the join ask for 
    }
    // Destroy mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;


}