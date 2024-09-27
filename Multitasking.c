#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

// Function to calculate the sum
long calculate_sum(long start, long end) {
    long sum = 0;
    for (long i = start; i < end; i++) {
        sum += i;
    }
    return sum;
}

// Multitasking with fork
void multitask_with_fork(long N, int num_tasks) {
    printf("Running multitasking with fork()...\n");

    pid_t pids[num_tasks];
    long sum = 0;
    int file_descriptor[2];

    // Create a pipe for inter-process communication
    if (pipe(file_descriptor) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_tasks; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {  // Child process
            close(file_descriptor[0]);  // Close reading end of the pipe
            long partial_sum = calculate_sum(i * (N / num_tasks), (i + 1) * (N / num_tasks));
            write(file_descriptor[1], &partial_sum, sizeof(partial_sum));  // Write the result to the pipe
            close(file_descriptor[1]);
            exit(0);  // Child exits
        }
    }

    close(file_descriptor[1]);  // Close the writing end of the pipe

    // Parent process waits for all child processes and accumulates results
    long partial_sum;
    for (int i = 0; i < num_tasks; i++) {
        wait(NULL);  // Wait for child processes
        read(file_descriptor[0], &partial_sum, sizeof(partial_sum));  // Read the result from the pipe
        sum += partial_sum;
    }

    close(file_descriptor[0]);

    printf("Final sum (fork): %ld\n", sum);
}

// Multitasking with popen
void multitask_with_popen(long N, int num_tasks) {
    printf("Running multitasking with popen()...\n");

    long sum = 0;
    FILE *file_pointer;
    char command[256];

    for (int i = 0; i < num_tasks; i++) {
        // Construct the command to run
        snprintf(command, sizeof(command), "./baseline %ld %ld", i * (N / num_tasks), (i + 1) * (N / num_tasks));
        
        // Run the baseline program for a specific range using popen()
        file_pointer = popen(command, "r");
        if (file_pointer == NULL) {
            perror("popen failed");
            exit(EXIT_FAILURE);
        }

        long partial_sum;
        fscanf(file_pointer, "%ld", &partial_sum);  // Read the result from the baseline program
        sum += partial_sum;

        pclose(file_pointer);
    }

    printf("Final sum (popen): %ld\n", sum);
}

int main(int argc, char *argv[]) {
    long N;
    int num_tasks;

    // Get user input for N and number of tasks
    printf("Enter the value of N: ");
    scanf("%ld", &N);
    printf("Enter the number of tasks: ");
    scanf("%d", &num_tasks);

    // Option 2: Measuring time with time.h library
    clock_t start_time, end_time;
    double cpu_time_used;
    
    // Multitasking with fork()
    start_time = clock();
    multitask_with_fork(N, num_tasks);
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken using fork(): %f seconds\n\n", cpu_time_used);

    // Multitasking with popen()
    start_time = clock();
    multitask_with_popen(N, num_tasks);
    end_time = clock();
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken using popen(): %f seconds\n", cpu_time_used);

    return 0;
}
