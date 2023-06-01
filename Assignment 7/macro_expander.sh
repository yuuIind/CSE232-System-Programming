#!/bin/bash

# name of the file to be written into a file
input_file="main.c"

# Set the name of the expanded file: "expanded_" + input_file
output_file="expanded_${input_file}"

# Write the code into a file
gcc -E "$input_file" -o "${output_file}"

# Compile program
gcc "${output_file}" -o "expanded_${input_file%.*}"

# Run program
./"expanded_${input_file%.*}"
