#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    unsigned long long start;
    unsigned long long end;
    unsigned long long sum;
} ThreadData;

void *calculate_sum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->sum = 0;
    for (unsigned long long i = data->start; i <= data->end; i++) {
        data->sum += i;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s N NUM_THREADS\n", argv[0]);
        return 1;
    }

    unsigned long long N = atoll(argv[1]);  // Total number up to which to sum
    int NUM_THREADS = atoi(argv[2]);        // Number of threads

    // Validate NUM_THREADS
    if (NUM_THREADS <= 0) {
        fprintf(stderr, "Error: NUM_THREADS must be positive.\n");
        return 1;
    }

    // Ensure NUM_THREADS evenly divides N
    if (N % NUM_THREADS != 0) {
        fprintf(stderr, "Error: NUM_THREADS must evenly divide N.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    unsigned long long workload = N / NUM_THREADS;

    // Start timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create threads and assign their workload
    unsigned long long current_start = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = current_start;
        thread_data[i].end = (i == NUM_THREADS - 1) ? N : current_start + workload - 1;  // Ensure the last thread goes to N
        current_start += workload; // Correct the starting point for the next thread
        pthread_create(&threads[i], NULL, calculate_sum, (void *)&thread_data[i]);
    }

    // Wait for all threads to finish
    unsigned long long total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].sum;
    }

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time taken in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    // Print the total sum and time taken
    printf("Total sum is: %llu\n", total_sum); // Print the total sum
    printf("Time taken is %lf seconds\n", time_taken);

    return 0;
}
