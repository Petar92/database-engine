#!/bin/bash

set -e

echo "Setting up C++ project with Conan and libpg_query..."

sudo apt-get update
sudo apt-get install -y build-essential cmake git python3-pip

pip3 install conan

conan profile detect --force

TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

make
sudo make install

sudo ldconfig

echo "Setup complete!"
echo "Now run: ./scripts/build.sh --conan"

cd /
rm -rf "$TEMP_DIR"