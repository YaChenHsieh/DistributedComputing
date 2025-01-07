
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// Define shared variables
#define NUM_THREADS 4 
#define ARRAY_SIZE 7

// Shared variables
int array[ARRAY_SIZE] = {2, 5, 6, 3, 8, 9, 7};
bool swapped = false; // Flag to check if any elements were swapped

// Mutex and condition variable
pthread_mutex_t mutex;
pthread_mutex_t sync_mutex; // for barrier
pthread_cond_t sync_cond;

// Barrier counter and generation counter
int sync_count = 0; 
int sync_generation = 0; // ** ensure threads to wait on the correct cycle of the barrier

// Barrier wait function
void barrier_wait() {
    pthread_mutex_lock(&sync_mutex);
    
    int my_generation = sync_generation;
    sync_count++;

    if (sync_count == NUM_THREADS) { // Last thread to reach the barrier
        printf("Thread %ld reached barrier and is resetting it. Generation: %d\n", pthread_self(), sync_generation); // Debug output
        sync_count = 0; // Reset counter for next use
        sync_generation++; // generation increase 
        pthread_cond_broadcast(&sync_cond); // Wake up all waiting threads
    } else {
        // Wait until all threads reach the barrier
        while (my_generation == sync_generation) {
            pthread_cond_wait(&sync_cond, &sync_mutex);
        }
    }
    
    pthread_mutex_unlock(&sync_mutex);
}

// Even-Odd sorting thread function
void* even_odd_sorted(void *arg) {
    int thread_id = *((int*)arg);
    bool local_swapped = false;

    while(1) {
        // Reset the global swapped flag at the start of each iteration
        pthread_mutex_lock(&mutex);
        swapped = false;
        pthread_mutex_unlock(&mutex);

        // Even phase
        local_swapped = false; // Initialize local_swapped for each phase
        for (int i = thread_id * 2; i < ARRAY_SIZE - 1; i += 2 * NUM_THREADS) { //[0,1]
            if(array[i] > array[i+1]) {
                // Swap elements
                int temp = array[i];
                array[i] = array[i+1];
                array[i+1] = temp;

                local_swapped = true;
                printf("Thread %d swapped %d and %d in even phase\n", thread_id, array[i], array[i+1]); // Debug output
            }
        }

        // Update the global swapped flag if any swap occurred in this phase
        pthread_mutex_lock(&mutex);
        if(local_swapped) {swapped = true;}
        pthread_mutex_unlock(&mutex);

        // Wait for all threads to finish the even phase
        barrier_wait();

        // Odd phase
        local_swapped = false; // Initialize local_swapped for each phase 
        for (int i = 1 + thread_id * 2; i < ARRAY_SIZE - 1; i += 2 * NUM_THREADS) { //[1,2]
            if(array[i] > array[i+1]) {
                // Swap elements
                int temp = array[i];
                array[i] = array[i+1];
                array[i+1] = temp;

                local_swapped = true;
                printf("Thread %d swapped %d and %d in odd phase\n", thread_id, array[i], array[i+1]); // Debug output
            }
        }

        // Update the global swapped
        pthread_mutex_lock(&mutex);
        if(local_swapped) {swapped = true;}
        pthread_mutex_unlock(&mutex);

        // Wait for all threads to finish the odd phase
        barrier_wait();

        // Final check to see if sorting is complete (Even+Odd)-> done = No swap (swapped:global)
        pthread_mutex_lock(&mutex);
        bool done = !swapped; 
        pthread_mutex_unlock(&mutex);

        // Wait at an additional barrier to ensure all threads check 'done' together
        barrier_wait(); // ***additional barrier to synchronize exit*** -> don't forget this

        if(done) { // If no swaps occurred, sorting is complete
            printf("Thread %d found array sorted and is exiting.\n", thread_id); // Debug output
            break;
        }
    }

    pthread_exit(NULL);
}

int main() {
    // Initialize mutexes
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex init failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&sync_mutex, NULL) != 0) {
        perror("Sync mutex init failed");
        exit(EXIT_FAILURE);
    }

    // Initialize condition variable
    if (pthread_cond_init(&sync_cond, NULL) != 0) {
        perror("Condition variable init failed");
        exit(EXIT_FAILURE);
    }

    // Declare threads and their IDs
    pthread_t threads[NUM_THREADS];
    int thread_id[NUM_THREADS];

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_id[i] = i;
        int ret = pthread_create(&threads[i], NULL, even_odd_sorted, &thread_id[i]);
        if (ret != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutexes and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&sync_mutex);
    pthread_cond_destroy(&sync_cond);

    // Print the sorted array
    printf("Sorted array: ");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
