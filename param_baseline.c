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
	unsigned long long sum = 0;
	unsigned long long upper = atoll(argv[1]);  // Total number up to which to sum
	unsigned long long lower = atoll(argv[2]);
    
    
	// Calculate sum
	for (unsigned long long i = lower; i <=upper; i++) {
    	sum += i;
	}
    
    
	// Print sum
	printf("Sum is: %llu\n", sum);
 
    
	return 0;
}
