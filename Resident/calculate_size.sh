#!/bin/bash

# Path to crt0.o
CRT0_PATH="/usr/local/lib/gcc/human68k/4.6.2/../../../../human68k/lib/crt0.o"

# Extract the size of crt0.o
CRT0_SIZE=$(size "$CRT0_PATH" | grep "$(basename "$CRT0_PATH")" | cut -f1)

# Calculate the aligned size (rounded up to the nearest multiple of 4)
ALIGNED_SIZE=$(( (CRT0_SIZE + 3) & ~3 ))

# Generate the header file
echo "#define CRT0_SIZE $ALIGNED_SIZE" > crt0_size.h