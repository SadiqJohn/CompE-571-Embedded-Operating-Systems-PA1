#!/bin/bash

# Constants for N, NUM_THREADS, and NUM_TASKS
N_VALUES=(100000000 1000000000 10000000000)
NUM_THREADS_VALUES=(2 4 8)
NUM_TASKS_VALUES=(2 4 8)
REPEAT=10  # Number of repetitions to ensure statistical significance
CSV_FILE="execution_times.csv"

# Function to calculate average
calculate_average() {
    sum=0
    for time in "${times[@]}"; do
        sum=$(echo "$sum + $time" | bc)
    done
    avg=$(echo "scale=5; $sum / $REPEAT" | bc)
    echo $avg
}

# Function to calculate standard deviation
calculate_stddev() {
    avg=$1
    sum_of_squares=0
    for time in "${times[@]}"; do
        diff=$(echo "$time - $avg" | bc)
        sq=$(echo "$diff^2" | bc)
        sum_of_squares=$(echo "$sum_of_squares + $sq" | bc)
    done
    variance=$(echo "scale=5; $sum_of_squares / $REPEAT" | bc)
    stddev=$(echo "scale=5; sqrt($variance)" | bc)
    echo $stddev
}

# Initialize CSV file and add headers
echo "Case,N,Threads/Tasks,Execution Times (s),Average Time (s),Std Dev (s)" > $CSV_FILE

# Run tests for each case
run_test() {
    case=$1
    N=$2
    THREADS_TASKS=$3
    program=$4

    # Array to store execution times
    times=()

    # Run the program $REPEAT times
    for ((i = 0; i < REPEAT; i++)); do
        start_time=$(date +%s.%N)
        if [[ "$case" == "multithreading" ]]; then
            ./$program $N $THREADS_TASKS
        elif [[ "$case" == "multitasking" ]]; then
            ./$program $N $THREADS_TASKS
        else
            ./$program $N
        fi
        end_time=$(date +%s.%N)
        elapsed=$(echo "$end_time - $start_time" | bc)
        times+=($elapsed)
    done

    # Calculate average and standard deviation
    avg=$(calculate_average)
    stddev=$(calculate_stddev $avg)

    # Join times array as a single string for CSV
    exec_times=$(IFS=, ; echo "${times[*]}")

    # Output results to the console
    echo "Average time: $avg seconds"
    echo "Standard deviation: $stddev seconds"

    # Append the data to the CSV file
    echo "$case,$N,$THREADS_TASKS,$exec_times,$avg,$stddev" >> $CSV_FILE
}

# Run baseline case
for N in "${N_VALUES[@]}"; do
    echo "Running baseline case for N=$N..."
    run_test "baseline" $N "" "baseline"
    echo
done

# Run multithreading case
for N in "${N_VALUES[@]}"; do
    for THREADS in "${NUM_THREADS_VALUES[@]}"; do
        echo "Running multithreading case for N=$N, NUM_THREADS=$THREADS..."
        run_test "multithreading" $N $THREADS "multithreading"
        echo
    done
done

# Run multitasking case
for N in "${N_VALUES[@]}"; do
    for TASKS in "${NUM_TASKS_VALUES[@]}"; do
        echo "Running multitasking case for N=$N, NUM_TASKS=$TASKS..."
        run_test "multitasking" $N $TASKS "multitasking"
        echo
    done
done

# Notify user that data is stored
echo "Execution time data has been stored in $CSV_FILE"
