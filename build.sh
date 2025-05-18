#!/bin/bash
# Build script for the Reminder application
set -e  # Exit on error

echo "Building Reminder application..."

# Clean any previous build files
rm -rf build

# Create build directory
mkdir -p build

# Change to build directory
cd build

# Get compiler flags
CXX_FLAGS=$(pkg-config --cflags gtkmm-3.0 libnotify ayatana-appindicator3-0.1)
LD_FLAGS=$(pkg-config --libs gtkmm-3.0 libnotify ayatana-appindicator3-0.1)

# Compile each source file
echo "Compiling source files..."
g++ -c ../src/main.cpp $CXX_FLAGS -I/usr/include/sqlite3
g++ -c ../src/reminder_app.cpp $CXX_FLAGS -I/usr/include/sqlite3
g++ -c ../src/reminder_popup_window.cpp $CXX_FLAGS -I/usr/include/sqlite3

# Link all objects
echo "Linking objects..."
g++ main.o reminder_app.o reminder_popup_window.o -o reminder $LD_FLAGS -lsqlite3 -lpthread

# Check if build was successful
if [ -f reminder ]; then
    echo "Build completed successfully! Executable is at build/reminder"
else
    echo "Build failed!"
    exit 1
fi
