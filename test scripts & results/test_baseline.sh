#!/bin/bash

# Bash script to test baseline.c with increasing N values and changing tolerance, logging results to a CSV

# Ensure baseline program is compiled
gcc -o baseline baseline.c -lrt

# Initialize starting N, tolerance, and CSV file
initial_N=1               # Starting value for N
max_N=10000000            # Maximum N value
N_increment_base=10       # Increment N by powers of 10
initial_tolerance=0.001   # Starting tolerance (in seconds)
tolerance_increment=0.1   # Percentage increment for tolerance after every 10 runs
run_count=0
csv_file="baseline_results_increment.csv"  # CSV file to store results

# Check if CSV file exists, if not create it and add headers
if [ ! -f $csv_file ]; then
    echo "Run, N, Sum, Time Taken (seconds), Tolerance" > $csv_file
fi

# Function to run the baseline and capture the sum and time taken
run_baseline() {
    result=$(./baseline $1)    # Run the baseline program with the given N
    sum=$(echo "$result" | grep "Sum is" | awk '{print $3}')     # Extract sum
    time_taken=$(echo "$result" | grep "Time taken" | awk '{print $4}')   # Extract time taken
    echo "Run $run_count: N=$1, Sum is $sum, Time taken is $time_taken seconds, Tolerance: $2"

    # Append result to CSV file
    echo "$run_count, $1, $sum, $time_taken, $2" >> $csv_file

    echo $time_taken
}

# Loop over N values, incrementing by powers of 10
N=$initial_N
while [ $N -le $max_N ]; do
    # Reset variables for each new N value
    prev_time=0
    tolerance=$initial_tolerance
    converged=false
    run_count_for_N=0

    # Run the baseline multiple times for the current N until times converge within tolerance
    while [ "$converged" = false ]; do
        run_count=$((run_count+1))
        run_count_for_N=$((run_count_for_N+1))
        current_time=$(run_baseline $N $tolerance)

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
            echo "Results for N=$N have converged. Stopping after $run_count_for_N runs."
        else
            prev_time=$current_time
        fi

        # Increase tolerance by a percentage every 10 runs
        if (( run_count_for_N % 10 == 0 )); then
            tolerance=$(echo "$tolerance + ($tolerance * $tolerance_increment)" | bc -l)
            echo "Increased tolerance to $tolerance after $run_count_for_N runs."
        fi
    done

    # Increment N to the next power of 10
    N=$((N * N_increment_base))
done

