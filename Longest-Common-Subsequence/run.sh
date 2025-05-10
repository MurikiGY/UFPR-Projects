#!/bin/bash

# ---
# Autor: Muriki G. Yamanaka
# Objective: Script generate table speedup from LCS code
# ---

# Define the numbers to iterate over
numbers=(10 25 50 75 100)

# Outer loop for the first number
for ((i=0; i<${#numbers[@]}; i++)); do
  # Inner loop for the second number, starting from the current index
  for ((j=i; j<${#numbers[@]}; j++)); do
    #echo "${numbers[i]} x ${numbers[j]} = $((numbers[i] * numbers[j]))"
    ./lcs ./inputs/fileA${numbers[i]}.in ./inputs/fileB${numbers[j]}.in
    sleep 1
  done
done



#./lcs ./inputs/fileA10.in ./inputs/fileB10.in
