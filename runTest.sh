#!/bin/bash

# Build the cmake project
./build.sh $@

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake build"
    exit ${E_CODE}
fi

BUILD_DIR="build"

# Parse script arguments
while [[ $# -gt 0 ]]; do
case $1 in
    -r|--release)
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

# Check if executable exist
if [ -e ${BUILD_DIR} ]; then
    cd ${BUILD_DIR}
    ctest @a
else
    echo "Error during test execution"
    exit 1
fi
