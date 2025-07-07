#!/bin/bash

# Default arguments
CLEAN_BUILD=0
BUILD_RELEASE=0

# Use the number of core as the default parallel argument
BUILD_CORES=$(cat /proc/cpuinfo | grep -c processor)

BUILD_DIR="build"

# Build preset to use
BUILD_PRESET="vcpkg"

# Find clang executable
if [ -z "${CLANG_CC}" ]; then
    CLANG_CC=clang
fi
if [ -z "${CLANG_CXX}" ]; then
    CLANG_CXX=clang++
fi

# Parse script arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -r|--release)
        BUILD_RELEASE=1
        shift
        ;;
    -c|--clean)
        CLEAN_BUILD=1
        shift
        ;;
    -p|--parellel)
        BUILD_CORES=$2
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

# Build commands arguments
CONFIG_ARGS="--preset ${BUILD_PRESET}"
BUILD_ARGS=""

# Build in debug mode if requested
if [ ${BUILD_RELEASE} -eq 0 ]; then
    CONFIG_ARGS="${CONFIG_ARGS} -DCMAKE_BUILD_TYPE=Debug "
else
    CONFIG_ARGS="${CONFIG_ARGS} -DCMAKE_BUILD_TYPE=Release "
fi 

# Use the given number of core for the build
BUILD_ARGS="${BUILD_ARGS} --parallel ${BUILD_CORES}"

# If a clean build is required delete the build folder if it exist
if [ -d "${BUILD_DIR}" ]; then
    if [ ${CLEAN_BUILD} -eq 1 ]; then
        rm -r ${BUILD_DIR}
        mkdir ${BUILD_DIR}
    fi
else
    mkdir ${BUILD_DIR}
fi

# Enter the build folder and start the build
SOURCE_DIR=${PWD}

export CC=${CLANG_CC}
export CXX=${CLANG_CXX}

cd ${BUILD_DIR}
cmake ${CONFIG_ARGS} ${SOURCE_DIR}

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake configuration"
    exit ${E_CODE}
fi

# Start the build
cmake --build . ${BUILD_ARGS}

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake build"
    exit ${E_CODE}
fi

exit 0
