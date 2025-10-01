#!/bin/bash
# chmod +x run_make.sh

# Exit immediately if a command exits with a non-zero status
set -e

# Running Lint To verify all programms are good to go for building
# and installation.
echo "Checking Lint"
./lint.sh

# Run the 'build' target in the Makefile
echo "Building kernel 'make build'..."
(cd ../kernel && make)

# Run the 'test' target in the Makefile
echo "Installing driver'..."
insmod ../kernel/build/nxp_simtemp.ko
