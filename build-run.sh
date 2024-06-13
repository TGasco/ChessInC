#!/bin/bash

# This script is used to build and run the C executable

# Build the C executable
echo "Building the C executable..."
make

# Run the C executable
echo "Running the C executable..."

# Run the C executable with the given arguments
./bin/chess "$@"