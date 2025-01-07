# DistributedComputing
This repository contains a collection of my homework assignments focusing on parallel algorithms, synchronization, and OpenMP. Each homework is documented with problem descriptions, challenges faced, and solutions implemented.

## Table of Contents

- [Homework 1](#homework-1)
- [Homework 2](#homework-2)
- [Homework 3](#homework-3)
- [Homework Extra](#homework-extra)
- [How to Run](#how-to-run)
- [License](#license)

## Homework 1

### Task

**Parallel Array Sorting Check**

- **Objective:** Determine if an array is sorted using parallel processing.
- **Challenge:** Uncertainty on how to implement parallel checking for array sorting.

### Solution

1. **Approach:**
   - **Chunking:** Divide the array into smaller chunks.
   - **Parallel Processing:** Assign each chunk to a separate thread to verify if it's sorted.
   - **Combination:** After all threads complete, combine the results to determine if the entire array is sorted.

2. **Implementation:**
   - Utilized multi-threading to handle each chunk independently.
   - Ensured thread synchronization when combining results to avoid race conditions.

## Homework 2

### Task

**Conditional Appearance Check**

1. **Q2-2:** Ensure that both `1` and `4` appear in the dataset.
2. **Implementation:** Create a function that returns `true` if both numbers appear, otherwise `false`. Use this function as a condition in the program.

## Homework 3

### Task

**Synchronization and Barrier Issues**

1. **Initialization Error:**
   - **Issue:** Forgot to initialize `local_swapped = false` in the Odd phase, causing an infinite loop.
   
2. **Barrier Synchronization:**
   - **Issue:** Initially used only one condition variable (`sync_count`) for the barrier, leading to an infinite loop.
   - **Solution:** Added an extra condition variable to prevent spurious wake-ups and implemented `sync_generation` to ensure threads are waiting on the correct barrier cycle.

### Solution

1. **Initialization Fix:**

```java
// Initialize local_swapped to false at the beginning of the Odd phase
local_swapped = false;
```

### Explanation

- **Spurious Wake-Up Prevention:**
  - **Problem:** A thread may wake up spuriously and see `sync_count` reset, incorrectly assuming the barrier condition is met.
  - **Solution:** Use `sync_generation` to track the barrier cycle, ensuring threads wait for the correct generation and preventing incorrect barrier condition checks.

## Homework Extra

### Task

**Implementing with OpenMP**

- **Objective:** Utilize OpenMP to parallelize specific tasks for performance improvement.

### Solution

1. **Using OpenMP Directives:**
   - Parallelized loops and sections to distribute work across multiple threads.
   - Managed synchronization and shared resources to avoid race conditions.

## How to Run

### Step 1: Compile

Use `gcc-14` with OpenMP support to compile the source files. Replace `file.c` with your actual source file name.

```bash
gcc-14 -fopenmp file.c -o file
```

### Step 2: Run Executable

Execute the compiled binary.

```bash
./file
```

### Compile and Run in One Step

Alternatively, compile and run the executable in a single command:

```bash
gcc-14 -fopenmp -o openmptest openmptest.c && ./openmptest
```

**Examples:**

```bash
gcc-14 -fopenmp -o find_ancestors_1 find_ancestors_1.c && ./find_ancestors_1
```

## License

This project is licensed under the MIT License.
