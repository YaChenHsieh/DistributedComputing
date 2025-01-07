#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

// define Constants
# define NUM_THREADS 4 // Number of threads to create.
# define ARRAY_SIZE 8 // Size of the array to check.


// Step1: Define the shared data 
// Step2: Create threads mechanism
//       -> 1. Setting Up Synchronization Primitives
//       -> 2. Create barrier_wait function: mutex & cond(count the  threads) to make sure the threads all reach barrier
//       -> 3. Create check_sorted function: 
//             -- use mutex for CS only 1 thread to modify is_sorted 
//             -- end of the function, need to barrier wait
// Step3: main(): 
//       -> 1. Init
//          -- threads 
//          -- mutex, barrier(mutex, cond)
//       -> 2. Create threads to run the task (check_sorted)
//       -> 3. Wait threads to join back ****Easy to forget
//       -> 4. Check if is_sorted
//       -> 5. Destory mutex, keys


// define shared data
int array[ARRAY_SIZE] = {1,2,3,4,5,6,7,8}; // use {} not []
// int array[ARRAY_SIZE] = {1,9,3,4,5,6,7,8}; // use {} not []
bool is_sorted = true; // Shared variable to track if array is sorted

//  threads mechanism -> 1. barrier 2. check_sorted
// Init Synchronization Primitives
pthread_mutex_t mutex; // mutext for CS -> is_sorted variable
pthread_mutex_t barrier_mutex; // pair with barrier_cond (mutex, cond) -> condition here is barrier count
pthread_cond_t barrier_cond;
int barrier_count = 0;

// barrier_wait func
void barrier_wait(){
    pthread_mutex_lock(&barrier_mutex); // lock : letting obly 1 thead modify count at a time
    barrier_count ++; 

    if (barrier_count == NUM_THREADS){ 
        barrier_count = 0; // Once all arrived, reset count for reuse
        pthread_cond_broadcast(&barrier_cond); // wake up all wait() threads **** forgot the & for barrier_cond
    }
    else {
        pthread_cond_wait(&barrier_cond, &barrier_mutex); // wait() and pass &barrier_mutex key to others
    }

    pthread_mutex_unlock(&barrier_mutex);
}

// check_sorted func
void* check_sorted(void* arg){ //*** Use pointer to check arr -> void* arg: passing pointer arg
    // init var
    int thread_id = *(int*) arg; // get the thread id
    int start = (ARRAY_SIZE/NUM_THREADS) * thread_id; // get start for each chunck 
    int end = (thread_id == NUM_THREADS-1) ? ARRAY_SIZE: start + (ARRAY_SIZE / NUM_THREADS); // get end chunck -> the LAST chuck(NUM_THREADS-1) might be bigger(bc the rest of the array) 

    // Adjust start for threads other than the first: bc: if the [1,9] [2,3,4] still return sorted
    if(thread_id != 0){
        start -= 1;
    }

    for(int i=start; i < end-1; ++i){ // end - 1 -> [i+1] will out of bound if using end || ++1 -> incremented first, then assign the new value: more efficient.
        if(array[i] > array[i+1]){
            pthread_mutex_lock(&mutex);
            is_sorted = false;
            pthread_mutex_unlock(&mutex);
            break; // -> break bc already not sorted, no need to continue
        }
    }

    // Wait for all threads back 
    barrier_wait();
    return NULL; // need to return 

}



// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

// pthread_t *thread
// Type: pthread_t *
// Description: This is a pointer to a pthread_t type variable used to store the unique identifier (ID) of the newly created thread. Can be used to perform subsequent operations on the thread, such as waiting for it to end.

// const pthread_attr_t *attr
// Type: const pthread_attr_t *
// Description: This is a pointer to the thread attribute object, used to set the attributes of the new thread (such as stack size, scheduling policy, etc.). If set to NULL, the default properties are used.

// void *(*start_routine) (void *)
// Type: function pointer
// Description: This is a pointer to a thread function. A new thread will start executing from this function. The function must accept a parameter of type void* and return a result of type void*.

// void *arg
// Type: void *
// Description: This is the parameter passed to the thread function. It can be a pointer of any type and needs to be type converted within the thread function.


int main(){ // return int
    // init threads and its id
    pthread_t threads[NUM_THREADS];
    int thread_id[NUM_THREADS];

    // init the mutex keys
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&barrier_mutex,NULL);
    pthread_cond_init(&barrier_cond,NULL);

    // Assigning Thread IDs to each threads
    // &threads[i] -> pthread_t : unique identifier for the ith thread created by pthread_create.
    // &thread_id[i] -> int : for passing the int to check_sort and can chunk the task (0,1,2)
    for (int i=0; i<NUM_THREADS; ++i){
        thread_id[i] = i;  // assign ID threads
        pthread_create(&threads[i],NULL,check_sorted,&thread_id[i]); // assign task to threads
    }

    for (int i=0; i<NUM_THREADS; ++i){
        pthread_join(threads[i],NULL); // task -> pthread_join expects a pthread_t, not a pthread_t *
    }

    //check sorted or not 
    if(is_sorted){
        printf("The array is sorted.\n");
    }
    else{
        printf("The array is NOT sorted.\n");
    }

    // destory keys
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&barrier_mutex);
    pthread_cond_destroy(&barrier_cond);

    return 0;
}


