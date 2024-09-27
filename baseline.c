#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide a value for N.\n");
        return 1;
    }

    // Initialize sum and N
    long long sum = 0;
    long long N = atoll(argv[1]);      // Total number up to which to sum
    
    // Monothonic clock is used to measure elapsed time
    // real time is used to measure wall clock time
    // Start timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Calculate sum
    // sum = N * (N - 1) / 2
    for (long long i = 0; i < N; i++) {
        sum += i;
    }
    
    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate time taken in seconds (with nanosecond precision)
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    // Print sum and time taken
    printf("Sum is: %lld\n", sum);
    printf("Time taken is %lf seconds\n", time_taken);
    
    return 0;
}