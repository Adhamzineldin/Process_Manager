#!/bin/bash

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build

# Build the programs
echo "Building programs..."
cd build
cmake ..
make

# Install the programs
echo "Installing programs..."
cp procmanager /usr/local/bin/
cp procmanager-gui /usr/local/bin/

# Install manual pages
echo "Installing manual pages..."
# Create man directory if it doesn't exist
mkdir -p /usr/local/share/man/man1

# Install manual pages
cp ../man/procmanager.1 /usr/local/share/man/man1/
cp ../man/procmanager-gui.1 /usr/local/share/man/man1/

# Compress manual pages
gzip -f /usr/local/share/man/man1/procmanager.1
gzip -f /usr/local/share/man/man1/procmanager-gui.1

# Update man database
echo "Updating man database..."
mandb

echo "Installation complete!"
echo "You can now use 'man procmanager' or 'man procmanager-gui' to view the manual pages." 