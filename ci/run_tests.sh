#!/bin/bash
# Builds and run unit tests with ASAN and UBSAN enabled.
################################################################################
SCRIPT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
REPO_ROOT="$(dirname "$SCRIPT_ROOT")"
CMAKE_PROJECT_ROOT="$REPO_ROOT"

BUILD_TYPE="Debug"
BUILD_ASAN="ON"
BUILD_UBSAN="ON"

# Double check that the repository root looks legit.
if [[ ! -f "$REPO_ROOT/.reporoot" ]] ; then
  echo "Could not find repository root dir: $REPO_ROOT" 1>&2
  exit 1
fi

# Create the build directory if it does not already exist, and the initialize it
# with CMake. Note that we only initialize the directory at creation time and
# not later because Ninja will do this check for us!
CI_BUILD_ROOT="$REPO_ROOT/buildout/ci-run_tests"
echo "CI build dir: $CI_BUILD_ROOT"

if [[ ! -d "$CI_BUILD_ROOT" ]] ; then
  echo "Build directory does not exist! Initializing..."

  # Create the directory.
  mkdir -p "$CI_BUILD_ROOT"

  # Initialize build directory with CMake.
  cmake \
    -Wdev \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DUSE_ADDRESS_SANITIZER="$BUILD_ASAN" \
    -DUSE_UNDEF_SANITIZER="$BUILD_UBSAN" \
    -GNinja \
    -S "$CMAKE_PROJECT_ROOT" -B "$CI_BUILD_ROOT"

  if [[ $? -ne 0 ]] ; then
      echo "CMake init build dir failed! Aborting..." 1>&2
      exit 1
  fi
fi

# Build the test target(s).
echo "Building..."
ninja -C "$CI_BUILD_ROOT" "test-shiny-runtime"

if [[ $? -ne 0 ]] ; then
    echo "Nina build failed! Aborting..." 1>&2
    exit 1
fi

# Run test target(s).
# (CTest doesn't have an option to change working directory which is annoying).
echo "Testing..."

(cd "$CI_BUILD_ROOT"; ctest --output-on-failure)

if [[ $? -ne 0 ]] ; then
    echo "Tests failed! Aborting..." 1>&2
    exit 1
fi
