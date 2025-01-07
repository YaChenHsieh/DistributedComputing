#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 7

// Init the mutex and cond and shared var
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int is_first_group_count = 0;

// Function to check if a thread is in the first group
int is_first_group(int thread_num){
    return (thread_num == 1 || thread_num == 4);
}

// Function to do the ordering
void* thread_func(void* arg){

    //assigne the thread num to thread
    int thread_num = *(int*)arg;

    if (is_first_group(thread_num)){
        // First group threads print immediately
        printf("Hello from thread %d\n", thread_num);

        // only 1 thread update shared val
        pthread_mutex_lock(&mutex);
        is_first_group_count++;

        // two of first group num done, wake up all wait threads
        if(is_first_group_count == 2){
            pthread_cond_broadcast(&cond);
        }
        
        pthread_mutex_unlock(&mutex);
    }
    else{
        // other threads wait umtil first group done
        pthread_mutex_lock(&mutex);

        //while first group not done yet, sleep
        while( is_first_group_count < 2 ){
            pthread_cond_wait(&cond, &mutex);
        }

        pthread_mutex_unlock(&mutex);
        // Now it's this thread's turn to print
        printf("Hello from thread %d\n", thread_num);

    }

    free(arg);
    return NULL;

}

int main(){
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS] = {1,2,3,4,5,6,7};

    for (int i =0; i<NUM_THREADS; ++i){
        
        // malloc space for num
        int* num = malloc(sizeof(int));
        if(num == NULL){
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        *num = thread_nums[i]; //assing numbers from the list
        if (pthread_create(&threads[i], NULL, thread_func, num) != 0 ){ // !=0 -> creation failed
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for(int i =0; i<NUM_THREADS; ++i){
        pthread_join(threads[i],NULL);
    }

    // Destroy mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
