#!/bin/bash

# Task 1
if [ ! -f "names".* ]
then
echo "Error: File not found."
exit 1
elif [ ! -f "salaries".* ]
then
echo "Error: File not found."
exit 1
fi

# Task 2
names_size=$(wc -c < names.*)
salaries_size=$(wc -c < salaries.*)
total_size=$((names_size + salaries_size))
echo "The size of 'names': $names_size"
echo "The size of 'salaries': $salaries_size"
echo "The total sizes of the two files: $total_size"

# Task 3
if [ $total_size -gt 100 ]
then
  echo "The total size is greater than 100."
  if [ $names_size -gt $salaries_size ]
  then
  rm names.*
  echo "Deleted file 'names'. Size = $names_size"
  else
  rm salaries.*
  echo "Deleted file 'salaries'. Size = $salaries_size"
  fi
fi