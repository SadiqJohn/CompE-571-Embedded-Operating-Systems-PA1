#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

//function to calculate the sum
unsigned long long sum(unsigned long long start, unsigned long long end)
{
  unsigned long long sum=0;
  unsigned long long i = 0;
  for(i=start; i<=end; i++)
  {
	sum+=i;
  }
  return sum;
}

int main(int argc, char *argv[])
{
  unsigned long long start=0;                   	//starting number for calculation
  unsigned long long end=0;                     	//ending number for calculation
  unsigned long long result=0;                  	//holds the sum result
  unsigned long long final_result=0;            	//holds the final result
  unsigned long long N= atoll(argv[1]);         	//N value
  int NUM_TASKS=2;                              	//number of tasks (2)
  unsigned long long WORKLOAD=N/NUM_TASKS;      	//Workload divided evenly among tasks
  clock_t Start, End;                           	//values to calculate the time
  int fd[2];                                    	//pipe file descriptor
  pipe(fd);                                     	//pipe for inter-process communication
  pid_t pid=fork();                             	//create fork
 
  Start=clock();                                	//start clock
 
  if(pid==0)                                    	//to differentiate each processes
  {
	close(fd[0]);                               	//close reading end
	start=0;                                    	//start value
	end=WORKLOAD;                               	//end value
	result=sum(start,end);                      	//holds result value from sum function           	 
	write(fd[1], &result, sizeof(result));      	//write the result value to the parent
	exit(0);                                    	//exit child
  }else if(pid>0)
  {
	close(fd[1]);                               	//close writing end
	start=1+(1*WORKLOAD);
	end=N;
	result=sum(start, end);
	final_result+=result;
	read(fd[0], &result, sizeof(result));       	//reads the child result
	final_result+=result;                       	//adds it to the final result
	printf("Final result= %llu\n", final_result);   //prints final result
  }
 
  End=clock();                                  	//end clock

  //calculate the time difference
  double time_taken= ((double)(End-Start))/CLOCKS_PER_SEC;
  printf("Time taken by program is: %f seconds\n", time_taken);      	//print the time taken
 
  return 0;
}
