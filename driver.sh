#!/bin/bash

# File: driver.sh

# Load Target Application from the JSON file
TARGET_APP=$(jq -r '.target' options.json)
# Load Prerequisites from the JSON file
PREREQUISITES=$(jq -r '.prerequisites[]' options.json)
# Load valid options and their requirements from the JSON file
VALID_OPTIONS=$(jq -r '.options[].aliases[]' options.json)

# Function to print the help message
print_help() {
  echo "Usage: $TARGET_APP [OPTIONS]"
  echo -e "Available options:\n"
  jq -r '.options[] | "\(.aliases | join(", "))\t\(.description)"' options.json
  exit 0
}

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo "jq command is required but not found. Please install jq to use this script."
    exit 1
fi

# Function to validate a file based on its type and extension
validate_file() {
  local file_path="$1"
  local allowed_extensions="$2"

  if [[ ! -f "$file_path" ]]; then
    echo "Error: File '$file_path' does not exist."
    exit 1
  fi

  # Check if the file extension is allowed
  local ext="${file_path##*.}"
  if [[ ! " ${allowed_extensions} " =~ " .$ext " ]]; then
    echo "Error: File type .$ext is not allowed. Allowed types are: ${allowed_extensions}"
    exit 1
  fi
}

# Function to validate options and their arguments
validate_options() {
  local skip_next=false

  for arg in "$@"; do
    if $skip_next; then
      skip_next=false
      continue
    fi

    # Check if the argument is a valid option
    if [[ "${VALID_OPTIONS}" =~ "${arg}" ]]; then
      # Check if the option requires an argument
      requires_arg=$(jq -r --arg opt "$arg" '.options[] | select(.aliases[] == $opt) | .requires_arg // false' options.json)
      
      if [[ "$requires_arg" == "true" ]]; then
        # Get the next argument (the argument for this option)
        next_arg="${2:-}"
        if [[ -z "$next_arg" || "$next_arg" == -* ]]; then
          echo "Error: Option $arg requires an argument."
          exit 1
        fi
        
        # Validate the argument if it's a file
        arg_type=$(jq -r --arg opt "$arg" '.options[] | select(.aliases[] == $opt) | .arg_type // empty' options.json)
        if [[ "$arg_type" == "file" ]]; then
          allowed_ext=$(jq -r --arg opt "$arg" '.options[] | select(.aliases[] == $opt) | .allowed_extensions[]?' options.json | tr '\n' ' ')
          validate_file "$next_arg" "$allowed_ext"
        fi

        # Skip the next argument (as it's been processed)
        skip_next=true
      fi
    else
      # Invalid option
      echo "Invalid option: $arg"
      print_help
    fi
  done
}

# Check if --help or -h is in the arguments
for arg in "$@"; do
  if [[ "$arg" == "--help" || "$arg" == "-h" ]]; then
    print_help
  fi
done

# Validate the options passed
validate_options "$@"

# Run the prerequisites prior to running the target application
for prereq in $PREREQUISITES; do
    if [[ -x "$prereq" ]]; then
        "$prereq"
    elif [[ "$prereq" == "make" ]]; then # Special case for make command
        make
    else
        echo "Error: Prerequisite '$prereq' is not executable."
        exit 1
    fi
done

# Print the command to be executed
echo "Running: $TARGET_APP $@"

# Run the target application with validated options
"$TARGET_APP" "$@"