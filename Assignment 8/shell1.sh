#!/bin/bash
# Muhammet Hakan Taştan 20190701009-071
# Step 1
awk -f awk1.awk file1.txt > out1.txt
awk -f awk1.awk file2.txt >> out1.txt

# Step 2
awk -f awk2.awk out1.txt