#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide a value for N.\n");
        return 1;
    }

    // Initialize sum and N
    unsigned long long sum = 0;
    unsigned long long N = atoll(argv[1]);  // Total number up to which to sum
    
    // Monothonic clock is used to measure elapsed time
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Calculate sum
    for (unsigned long long i = 0; i < N; i++) {
        sum += i;
    }
    
    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Calculate time taken in seconds (with nanosecond precision)
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    
    // Print sum and time taken
    printf("Sum is: %llu\n", sum);
    printf("Time taken is %lf seconds\n", time_taken);
    
    return 0;
}
