#!/bin/bash

set -e

PROJECT_ROOT=$(dirname $(dirname $(readlink -f $0)))
BUILD_DIR="$PROJECT_ROOT/build"

BUILD_TYPE="Development"
PACKAGE_MANAGER="system"
CLEAN_BUILD=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --conan)
            PACKAGE_MANAGER="conan"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "Building with: $BUILD_TYPE, Package manager: $PACKAGE_MANAGER"

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

case $PACKAGE_MANAGER in
    "conan")
        echo "Installing dependencies with Conan..."
        conan install .. --output-folder=. --build=missing
        cmake .. --preset conan
        ;;
    *)
        echo "Using system dependencies..."
        cmake .. --preset default
        ;;
esac

echo "Building project..."
cmake --build . --config $BUILD_TYPE -j$(nproc)

echo "Build completed successfully!"
echo "Executable: $BUILD_DIR/database-engine"