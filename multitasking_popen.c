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

	// Initialize sum and N
	unsigned long long partial_result = 0;
	unsigned long long final_result = 0;
	unsigned long long N = atoll(argv[1]);  // Total number up to which to sum
	int Num_task = atoi(argv[2]);
	unsigned long long upper=0;
	unsigned long long lower=0;
	unsigned long long WORKLOAD=N/Num_task;
	char command[256];
	FILE *fp;
    

	for (unsigned long long i = 0; i < Num_task; i++) {
    	lower=(i*WORKLOAD)+1;
    	upper=(i+1)*WORKLOAD;
    	snprintf(command, sizeof(command), "./param_baseline.o %llu %llu", lower, upper);
    	fp=popen(command, "r");
    	if(fp==NULL){
    	perror("open");
    	exit(EXIT_FAILURE);
    	}
    	fscanf(fp, "%llu", &partial_result);
    	final_result+=partial_result;    
    	pclose(fp);
	}
    
    
	// Print sum and time taken
	printf("Final result is: %llu\n", final_result);
 
    
	return 0;
}

