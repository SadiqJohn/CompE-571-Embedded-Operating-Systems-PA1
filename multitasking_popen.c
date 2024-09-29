#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
    	printf("Please provide a value for N.\n");
    	return 1;
	}

	unsigned long long partial_result = 0;
	unsigned long long final_result = 0;
	unsigned long long N = atoll(argv[1]);              	//N value from command line
	int Num_task = atoi(argv[2]);                       	//Num_task from command line
	unsigned long long upper=0;                         	//upper limit for calculation
	unsigned long long lower=0;                         	//lower limit for calculation
	unsigned long long WORKLOAD=N/Num_task;             	//Workload divided evenly among Num_task
	char command[256];
	FILE *fp;                                           	//file pointer
    
	// Start timer
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
    
	//to create new tasks and to read result values
	for (int i = 0; i < Num_task; i++) {
    	lower=(i*WORKLOAD)+1;
    	upper=(i+1)*WORKLOAD;
    	snprintf(command, sizeof(command), "./param_baseline.o %llu %llu", lower, upper);  //create command
    	fp=popen(command, "r");                              	//open pipe for inter-process communication
    	fscanf(fp, "%llu", &partial_result);                 	//read the partial result
    	final_result+=partial_result;                        	//add to the final result
	}
  	 
 	// End timer
	clock_gettime(CLOCK_MONOTONIC, &end);
    
	// Print sum
	printf("Final result is: %llu\n", final_result);
    
 	// Calculate time taken in seconds
	double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	printf("Time taken using popen(): %f seconds\n\n", time_taken);
 
    
	return 0;
}



