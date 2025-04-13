#!/bin/bash

set -e

PROJECT_DIR="$(dirname "$(readlink -f "$0")")"
BUILD_DIR="$PROJECT_DIR/build-wsl"

# 🧹 Clean build directory if "clean" is passed
if [[ "$1" == "clean" ]]; then
  echo "🧼 Cleaning build directory: $BUILD_DIR"
  rm -rf "$BUILD_DIR"
  echo "✅ Clean complete!"
  exit 0
fi

echo "🔧 Building in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "📦 Running CMake..."
cmake ..

echo "🔨 Building..."
cmake --build .

# ✅ Run tests if CTest config exists
if [[ -f "$BUILD_DIR/CTestTestfile.cmake" ]]; then
  echo "🧪 Running tests..."
  ctest --output-on-failure
else
  echo "⚠️ No CTest file found, skipping tests."
fi

echo "✅ Build (and test) complete!"
