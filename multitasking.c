#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// Function to calculate the sum in a given range
long calculate_sum(long start, long end) {
    long sum = 0;
    for (long i = start; i < end; i++) {
        sum += i;
    }
    return sum;
}

// Multitasking with fork()
void multitask_with_fork(long N, int num_tasks) {
    printf("Running multitasking with fork()...\n");

    pid_t pids[num_tasks];
    long sum = 0;
    int fd[2];  // File descriptor for pipe

    // Create a pipe for inter-process communication
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_tasks; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {  // Child process
            close(fd[0]);  // Close the reading end of the pipe
            long partial_sum = calculate_sum(i * (N / num_tasks), (i + 1) * (N / num_tasks));
            write(fd[1], &partial_sum, sizeof(partial_sum));  // Write the result to the pipe
            close(fd[1]);  // Close the writing end after use
            exit(0);  // Child exits after completing the task
        }
    }

    close(fd[1]);  // Close the writing end in the parent process

    // Parent process accumulates the results from child processes
    long partial_sum;
    for (int i = 0; i < num_tasks; i++) {
        wait(NULL);  // Wait for child processes to complete
        read(fd[0], &partial_sum, sizeof(partial_sum));  // Read the result from the pipe
        sum += partial_sum;  // Accumulate the sum
    }

    close(fd[0]);  // Close the reading end of the pipe

    printf("Final sum (fork): %ld\n", sum);
}

int main(int argc, char *argv[]) {
    // Check for correct number of arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s N NUM_THREADS\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    long long N = atoll(argv[1]);      // Total number up to which to sum
    int NUM_THREADS = atoi(argv[2]);     // Number of tasks (child processes)

    // Validate NUM_TASKS
    if (NUM_THREADS <= 0) {
        fprintf(stderr, "Error: NUM_THREADS must be positive.\n");
        return 1;
    }

    // Ensure NUM_TASKS evenly divides N
    if (N % NUM_THREADS != 0) {
        fprintf(stderr, "Error: NUM_THREADS must evenly divide N.\n");
        return 1;
    }

    // ** Fork() **

    // Start timer
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Multitasking with fork()
    multitask_with_fork(N, num_tasks);

    // End timer
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time taken in seconds (with nanosecond precision)
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken using fork(): %f seconds\n\n", time_taken);

    return 0;
}
