#!/bin/bash

# ---
# Autor: Muriki G. Yamanaka
# Objective: Script generate table speedup from LCS code
# ---

# Define the numbers to iterate over
sizes=(10 25 50 75 100)
#sizes=(10 25)

# Define threads to test
threads=(1 2 4 8)
#threads=(2 4)

# Create results directory
mkdir -p results

# ----- Run LCS

# Run sequential
mkdir -p results/result_original
# Outer loop for sizeA
for ((i=0, l=0; i<${#sizes[@]}; i++)); do
  # Inner loop for sizeB
  for ((j=i; j<${#sizes[@]}; j++, l++)); do

    for ((k=0; k<20 ;k++)); do
      echo "Thrds[original] Categ[$l]: ${sizes[i]} x ${sizes[j]}"
      ./lcs ./inputs/fileA${sizes[i]}.in ./inputs/fileB${sizes[j]}.in 2>> results/result_original/categ_$l.dat
      sleep 0.1
    done
  done
done

# Run parallel
for ((m=0; m<${#threads[@]}; m++)); do
  # Create directory
  mkdir -p results/result_${threads[m]}

  # Outer loop for sizeA
  for ((i=0, l=0; i<${#sizes[@]}; i++)); do
    # Inner loop for sizeB
    for ((j=i; j<${#sizes[@]}; j++, l++)); do

      for ((k=0; k<20 ;k++)); do
        echo "Thrds[${threads[m]}] Categ[$l]: ${sizes[i]} x ${sizes[j]}"
        ./lcs ./inputs/fileA${sizes[i]}.in ./inputs/fileB${sizes[j]}.in ${threads[m]} 2>> results/result_${threads[m]}/categ_$l.dat
        sleep 0.1
      done

    done
  done
done

## ----- Generate results

# Generate sequential
truncate -s 0 ./results/time_original
for file in $(ls -v ./results/result_original/); do
  filepth="./results/result_original/$file"
  
  # Calculate Mean
  mean=$(awk '{sum += $1; count++} END {if (count > 0) print sum / count}' $filepth)
  formatted_mean=$(printf "%.1f" "$mean")
  #printf "$formatted_mean\t" >> ./results/time_original
  printf "$formatted_mean\n" >> ./results/time_original

  ## Calculate Standard Deviation
  #stddev=$(awk '{sum += $1; sumsq += $1 * $1; count++} END {if (count > 0) print sqrt(sumsq / count - (sum / count) ^ 2)}' $filepth)
  #formatted_stddev=$(printf "%.1f" "$stddev")
  #printf "($formatted_stddev)\n" >> ./results/time_original
done

# Generate parallel
for ((m=0; m<${#threads[@]}; m++)); do
  printf "Thread(${threads[m]})\n"
  truncate -s 0 ./results/time_${threads[m]}
  for file in $(ls -v ./results/result_${threads[m]}/); do
    filepth="./results/result_${threads[m]}/$file"

    # Calculate Mean
    mean=$(awk '{sum += $1; count++} END {if (count > 0) print sum / count}' $filepth)
    formatted_mean=$(printf "%.1f" "$mean")
    #printf "$formatted_mean\t" >> ./results/time_${threads[m]}
    printf "$formatted_mean\n" >> ./results/time_${threads[m]}
    
    ## Calculate Standard Deviation
    #stddev=$(awk '{sum += $1; sumsq += $1 * $1; count++} END {if (count > 0) print sqrt(sumsq / count - (sum / count) ^ 2)}' $filepth)
    #formatted_stddev=$(printf "%.1f" "$stddev")
    #printf "($formatted_stddev)\n" >> ./results/time_${threads[m]}
  done
done

# Generate speedup and eficiency
for ((m=0; m<${#threads[@]}; m++)); do
  printf "Thread(${threads[m]})\n"

  # Calculate speedup
  truncate -s 0 ./results/speedup_${threads[m]}
  paste results/time_original results/time_${threads[m]} | awk '{if ($2 != 0) printf "%.1f\n", $1 / $2; else print "Division by zero"}' >> ./results/speedup_${threads[m]}

  # Calculate Eficiency
  truncate -s 0 ./results/eficiency_${threads[m]}
  awk -v constant=${threads[m]} '{printf "%.1f\n", $1 / constant}' ./results/speedup_${threads[m]} >> ./results/eficiency_${threads[m]}
done

printf "S(1)\tE(1)\tS(2)\tE(2)\tS(4)\tE(4)\tS(8)\tE(8)\n"
paste ./results/speedup_1 ./results/eficiency_1 ./results/speedup_2 ./results/eficiency_2 \
      ./results/speedup_4 ./results/eficiency_4 ./results/speedup_8 ./results/eficiency_8
