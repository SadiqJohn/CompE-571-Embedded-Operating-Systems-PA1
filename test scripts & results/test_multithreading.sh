#!/bin/bash

# Bash script to test your C program with increasing N values and changing tolerance, logging results to a CSV

# Ensure your multithreading program is compiled
gcc -pthread -o multithreading multithreading.c

# Initialize starting N, tolerance, and CSV file
initial_N=1               # Starting value for N
max_N=10000000            # Maximum N value
N_increment_base=10       # Increment N by powers of 10
initial_tolerance=0.001   # Starting tolerance (in seconds)
tolerance_increment=0.1   # Percentage increment for tolerance after every 10 runs
run_count=0
csv_file="multithreading_results_increment.csv"    # CSV file to store results

# Check if CSV file exists, if not create it and add headers
if [ ! -f $csv_file ]; then
    echo "Run,N,NUM_THREADS,Sum,Time_Taken(s),Tolerance" > $csv_file
fi

# Function to run the program and capture the sum and time taken
run_program() {
    result=$(./multithreading $1 $2)    # Run the program with N and NUM_THREADS

    # Check if the program exited with an error
    if echo "$result" | grep -q "Error"; then
        echo "Error encountered for N=$1, NUM_THREADS=$2. Skipping."
        return 1
    fi

    sum=$(echo "$result" | grep "Sum is" | awk '{print $3}')           # Extract sum
    time_taken=$(echo "$result" | grep "Time taken is" | awk '{print $4}')   # Extract time taken

    # Calculate expected sum
    expected_sum=$(( ( $1 - 1 ) * $1 / 2 ))

    # Validate the sum
    if [ "$sum" -ne "$expected_sum" ]; then
        echo "Mismatch in sum for N=$1, NUM_THREADS=$2. Expected $expected_sum, got $sum. Skipping."
        return 1
    fi

    echo "Run $run_count: N=$1, NUM_THREADS=$2, Sum is $sum, Time taken is $time_taken seconds, Tolerance: $3"

    # Append result to CSV file
    echo "$run_count,$1,$2,$sum,$time_taken,$3" >> $csv_file

    echo $time_taken
}

# Array of NUM_THREADS values to test
THREAD_VALUES=(1 2 4 8 16)

# Loop over N values, incrementing by powers of 10
N=$initial_N
while [ $N -le $max_N ]; do
    for NUM_THREADS in "${THREAD_VALUES[@]}"
    do
        # Skip invalid combinations where NUM_THREADS is greater than N
        if (( NUM_THREADS > N )); then
            continue
        fi

        # Ensure NUM_THREADS evenly divides N
        if (( N % NUM_THREADS != 0 )); then
            echo "Skipping N=$N, NUM_THREADS=$NUM_THREADS because NUM_THREADS does not evenly divide N."
            continue
        fi

        # Reset variables for each new N and NUM_THREADS combination
        prev_time=0
        tolerance=$initial_tolerance
        converged=false
        run_count_for_N=0

        # Run the program multiple times for the current N and NUM_THREADS until times converge within tolerance
        while [ "$converged" = false ]; do
            run_count=$((run_count+1))
            run_count_for_N=$((run_count_for_N+1))
            current_time=$(run_program $N $NUM_THREADS $tolerance)

            # Check if the run_program function encountered an error
            if [ $? -ne 0 ]; then
                # An error occurred, skip to the next combination
                break
            fi

            # Skip comparison for the first run
            if [ $run_count_for_N -eq 1 ]; then
                prev_time=$current_time
                continue
            fi

            # Calculate the absolute difference between the last two runs
            diff=$(echo "$current_time - $prev_time" | bc -l)
            abs_diff=$(echo "if($diff<0) -1*$diff else $diff" | bc -l)

            # Check if the difference is within the tolerance
            if (( $(echo "$abs_diff < $tolerance" | bc -l) )); then
                converged=true
                echo "Results for N=$N, NUM_THREADS=$NUM_THREADS have converged after $run_count_for_N runs."
            else
                prev_time=$current_time
            fi

            # Increase tolerance by a percentage every 10 runs
            if (( run_count_for_N % 10 == 0 )); then
                tolerance=$(echo "$tolerance + ($tolerance * $tolerance_increment)" | bc -l)
                echo "Increased tolerance to $tolerance after $run_count_for_N runs."
            fi
        done
    done

    # Increment N to the next power of 10
    N=$((N * N_increment_base))
done
