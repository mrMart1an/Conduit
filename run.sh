#!/bin/bash

# Build the cmake project
./build.sh $@

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake build"
    exit ${E_CODE}
fi

BUILD_BASE_DIR="build"
BUILD_DIR="${BUILD_BASE_DIR}/Debug"

# Parse script arguments
while [[ $# -gt 0 ]]; do
case $1 in
    -r|--release)
        BUILD_DIR="${BUILD_BASE_DIR}/Release"
        shift
        ;;
    -c|--clean)
        shift
        ;;
    -p|--parellel)
        shift
        shift
        ;;
    -*|--*)
        echo "Unknown option $1"
        exit 1
        ;;
   *)
        BUILD_DIR="$1" # save build directory argument
        shift
        ;;
  esac
done

EXE_PATH="${BUILD_DIR}/examples/sandbox/Sandbox"

# Check if executable exist
if [ -e ${EXE_PATH} ]; then
    ./${EXE_PATH}
else
    echo "Couldn't find executable file: ${EXE_PATH}"
    exit 1
fi
