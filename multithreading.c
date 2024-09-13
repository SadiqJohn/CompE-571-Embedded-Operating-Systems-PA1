#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4 // Number of threads

// Structure to pass data to threads
typedef struct {
    int start;
    int end;
    long sum;
} ThreadData;

// Thread function to calculate partial sum
void *calculate_sum(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long sum = 0;
    for (int i = data->start; i < data->end; i++) {
        sum += i;
    }
    data->sum = sum;  // Store the partial sum in the struct
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide a value for N.\n");
        return 1;
    }
    
    // Initialize N from user input
    long long N = strtoll(argv[1], NULL, 10);  // Convert input to long long

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int workload = N / NUM_THREADS; // Divide the workload evenly
    int remainder = N % NUM_THREADS; // Handle the remainder if N is not divisible

    struct timespec start, end;
    
    // Start timer
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Create threads and assign their workload
    int current_start = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = current_start;
        // Distribute extra workload to the first 'remainder' threads
        thread_data[i].end = current_start + workload + (i < remainder ? 1 : 0);
        current_start = thread_data[i].end; // Update the start for the next thread
        if (pthread_create(&threads[i], NULL, calculate_sum, (void *)&thread_data[i])) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to finish
    long long total_sum = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL)) {
            printf("Error joining thread %d\n", i);
            return 1;
        }
        total_sum += thread_data[i].sum;
    }
    
    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate time taken in seconds (with nanosecond precision)
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    // Print sum and time taken, ensure output is flushed
    printf("Sum is: %lld\n", total_sum);
    printf("Time taken is %lf seconds\n", time_taken);
    fflush(stdout);  // Ensure output is flushed to the terminal
    
    return 0;
}
