#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 11  // Number of elements, and also the thread will process

// find the final ancestor function -> using recursion pointer jump
int find_final_ancestor(int index, int *arr, int *final_ancestors) {

    // If the final ancestor has already been computed, return it
    if (final_ancestors[index] != -1) { // in begining assign -1 (in C, -1 means not success)
        return final_ancestors[index];
    }

    // If the element points to itself, it is its own ancestor
    if (arr[index] == index) {
        final_ancestors[index] = index;
    } else {
        // Recursively find the ancestor of the parent
        final_ancestors[index] = find_final_ancestor(arr[index], arr, final_ancestors);
    }

    return final_ancestors[index];
}

int main() {
    // Init the given array -> (index 0 is unused) just for easier to track the index 
    int arr[N + 1] = {0, 4, 3, 4, 4, 4, 4, 8, 8, 8, 8, 10};
    
    // Array to store final ancestors -> will init to -1 (means not yet computed)
    int final_ancestors[N + 1];
    
    // Initiate all of the elements in final_ancestors to -1 
    for (int i = 1; i <= N; i++) {
        final_ancestors[i] = -1;
    }

    // Parallelized loop using OpenMP to compute final ancestors
    #pragma omp parallel for
    for (int i = 1; i <= N; i++) {
        // Each thread will compute the final ancestor for an index
        find_final_ancestor(i, arr, final_ancestors);
    }

    // Display the final ancestors
    printf("Final Ancestors:\n");
    for (int i = 1; i <= N; i++) {
        printf("%d ", final_ancestors[i]);
    }
    printf("\n");

    return 0;
}
