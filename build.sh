#!/bin/bash

# Set environmental variables
source .env

# Check if Conan exist
if ! [ -x "$(command -v conan)" ]; then
    echo "Error: conan isn't install"
    exit 1
fi

# Check if CMake exist
if ! [ -x "$(command -v cmake)" ]; then
    echo "Error: cmake isn't install"
    exit 1
fi

# Default arguments
CLEAN_BUILD=0
BUILD_RELEASE=0

# Use the number of core as the default parallel argument
BUILD_CORES=$(cat /proc/cpuinfo | grep -c processor)

BUILD_BASE_DIR="build"

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

# Build preset to use
if [ ${BUILD_RELEASE} -eq 0 ]; then
    BUILD_PRESET="conan-debug"
    CONAN_PROFILE="debug"
    BUILD_DIR="${BUILD_BASE_DIR}/Debug"
else
    BUILD_PRESET="conan-release"
    CONAN_PROFILE="default"
    BUILD_DIR="${BUILD_BASE_DIR}/Release"
fi 

# If a clean build is required delete the build folder if it exist
if [ -d "${BUILD_DIR}" ]; then
    if [ ${CLEAN_BUILD} -eq 1 ]; then
        rm -r ${BUILD_DIR}
        mkdir ${BUILD_DIR}
    fi
else
    mkdir ${BUILD_DIR}
fi

# Determine if Conan install need to run 
# Conan is run only if the edit time of the conanfile is
# more recent than the edit time of the Conan generators
# If the generators folder doesn't exist yet Conan is run

CONAN_INSTALL=0

if [ -d "${BUILD_DIR}/generators" ]; then
    CONAN_FILE_LAST_EDIT=$(stat -c %Y "conanfile.py")
    GENERATORS_FILE_LAST_EDIT=$(stat -c %Y "${BUILD_DIR}/generators")

    if (( CONAN_FILE_LAST_EDIT >= GENERATORS_FILE_LAST_EDIT )); then
        touch "${BUILD_DIR}/generators"
        CONAN_INSTALL=1
    fi
else
    CONAN_INSTALL=1
fi

# Run Conan install if necessary
if [ ${CONAN_INSTALL} -ne 0 ]; then
    conan install . --build=missing --profile=${CONAN_PROFILE}
fi

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error: during conan install"
    exit ${E_CODE}
fi

# Build commands arguments
CONFIG_ARGS="-G Ninja --preset ${BUILD_PRESET}"
BUILD_ARGS=""

# Use the given number of core for the build
BUILD_ARGS="${BUILD_ARGS} --parallel ${BUILD_CORES}"

# Enter the build folder and start the build
SOURCE_DIR=${PWD}

cd ${BUILD_DIR}
source ./generators/conanbuild.sh
cmake ${CONFIG_ARGS} ${SOURCE_DIR}

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake configuration"
    source ./generators/deactivate_conanbuild.sh
    exit ${E_CODE}
fi

# Start the build
cmake --build . ${BUILD_ARGS}

# Check for error
E_CODE=$?
if [ ${E_CODE} -ne 0 ]; then
    echo "Error during cmake build"
    source ./generators/deactivate_conanbuild.sh
    exit ${E_CODE}
fi

source ./generators/deactivate_conanbuild.sh

# Copy the compile commands for clangd
cp "compile_commands.json" "${SOURCE_DIR}/${BUILD_BASE_DIR}/compile_commands.json" 

exit 0
