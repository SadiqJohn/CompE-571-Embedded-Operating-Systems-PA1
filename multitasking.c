#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// Function to calculate the sum in a given range
long double calculate_sum(unsigned long long start, unsigned long long end) {
    long double sum = 0;
    for (unsigned long long i = start; i <= end; i++) {
        sum += i;
    }
    return sum;
}

// Multitasking with fork()
void multitask_with_fork(unsigned long long N, int NUM_TASKS) {
    printf("Running multitasking with fork()...\n");

    pid_t pids[NUM_TASKS];
    long double total_sum = 0;
    int fd[2];  // File descriptor for pipe

    // Create a pipe for inter-process communication
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_TASKS; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {  // Child process
            close(fd[0]);  // Close the reading end of the pipe
            unsigned long long start = i * (N / NUM_TASKS);
            unsigned long long end = (i == NUM_TASKS - 1) ? N : (i + 1) * (N / NUM_TASKS) - 1;
            long double partial_sum = calculate_sum(start, end);
            write(fd[1], &partial_sum, sizeof(long double));
            close(fd[1]);  // Close the writing end of the pipe
            exit(0);
        }
    }

    close(fd[1]);  // Close the writing end of the pipe in the parent

    // Read partial sums from the child processes
    long double partial_sum;
    for (int i = 0; i < NUM_TASKS; i++) {
        read(fd[0], &partial_sum, sizeof(long double));
        total_sum += partial_sum;
    }

    close(fd[0]);  // Close the reading end of the pipe

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_TASKS; i++) {
        wait(NULL);
    }

    // Print the total sum
    printf("Total sum is: %Lf\n", total_sum);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s N NUM_TASKS\n", argv[0]);
        return 1;
    }

    unsigned long long N = atoll(argv[1]);  // Total number up to which to sum
    int NUM_TASKS = atoi(argv[2]);          // Number of tasks (processes)

    // Start timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Perform multitasking
    multitask_with_fork(N, NUM_TASKS);

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time taken in seconds
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken is %lf seconds\n", time_taken);

    return 0;
}
