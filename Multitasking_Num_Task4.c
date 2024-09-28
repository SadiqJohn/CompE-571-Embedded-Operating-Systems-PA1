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
  unsigned long long start=0;               	//starting number for calculation
  unsigned long long end=0;                 	//ending number for calculation
  unsigned long long result=0;              	//holds the sum result
  unsigned long long final_result=0;        	//holds the final result
  unsigned long long N= atoll(argv[1]);     	//N value
  int NUM_TASKS=4;                          	//number of tasks (4)
  unsigned long long WORKLOAD=N/NUM_TASKS;  	//Workload divided evenly among tasks
  clock_t Start, End;                       	//values to calculate the time
  int fd1[2], fd2[2], fd3[2];               	//pipe file descriptor
  pipe(fd1);                                	//pipe for inter-process communication
  pipe(fd2);
  pipe(fd3);
  pid_t pid1=fork();                        	//create fork
  pid_t pid2=fork();
 
  Start=clock();                            	//start clock
 
  if(pid1>0 && pid2==0)                     	//to differentiate each processes
  {
	close(fd1[0]);                          	//close reading end
	start=0;                                	//start value
	end=WORKLOAD;                           	//end value
	result=sum(start,end);                  	//holds result value from sum function
	write(fd1[1], &result, sizeof(result)); 	//write the result value to the parent
	exit(0);                                	//exit child
  }else if(pid1==0 && pid2>0)
  {
	close(fd2[0]);
	start=1+(1*WORKLOAD);
	end=2*WORKLOAD;
	result=sum(start,end);
	write(fd2[1], &result, sizeof(result));
	exit(0);
  }else if(pid1==0 && pid2==0)
  {
	close(fd3[0]);
	start=1+(2*WORKLOAD);
	end=3*WORKLOAD;
	result=sum(start,end);
	write(fd3[1], &result, sizeof(result));
	exit(0);
  }else if(pid1>0 && pid2>0)
  {
	close(fd1[1]);                         	//close writing end
	close(fd2[1]);
	close(fd3[1]);
	start=1+(3*WORKLOAD);
	end=N;
	result=sum(start, end);
	final_result+=result;                 	 
	read(fd1[0], &result, sizeof(result)); 	//reads the child result
	final_result+=result;                  	//adds it to the final result
	read(fd2[0], &result, sizeof(result));
	final_result+=result;
	read(fd3[0], &result, sizeof(result));
	final_result+=result;
	printf("Final result= %llu\n", final_result);  //prints final result
  }
 
  End=clock();                             	//end clock                             	 
 
  //calculate the time difference
  double time_taken= ((double)(End-Start))/CLOCKS_PER_SEC;        	 
  printf("Time taken by program is: %f seconds\n", time_taken);    	//print the time taken
 
  return 0;
}
