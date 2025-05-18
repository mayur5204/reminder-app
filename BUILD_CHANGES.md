# Reminder App Build System Changes

## Summary of Changes

We've simplified the build system for the Reminder application by:

1. **Removing CMake dependency**:
   - Simplified the build process to directly use g++ for compilation
   - Removed all CMake-related files and configuration

2. **Streamlined build scripts**:
   - Single `build.sh` script handles all compilation
   - Single `install.sh` script manages local installation
   - Single `create-deb.sh` script creates a Debian package

3. **Fixed visual issues**:
   - Removed problematic "always on top" property which was causing build errors
   - Enhanced checkbox UI for marking reminders complete
   - Improved styling in the popup window

## Available Scripts

1. **./build.sh**
   - Builds the application directly using g++
   - Creates the executable at `build/reminder`

2. **./install.sh**
   - Installs the application locally in your user directory
   - Sets up desktop integration and autostart

3. **./create-deb.sh**
   - Creates a Debian package for system-wide installation
   - Generates `reminder_1.0-1_amd64.deb`

4. **./uninstall.sh**
   - Removes all installations of the application (both local and system-wide)
   - Provides options to keep or remove your reminder data
   - Handles cleanup of desktop integration and systemd services

## How to Use

```bash
# Build the application
./build.sh

# Install locally
./install.sh

# Create a Debian package
./create-deb.sh
```

## Troubleshooting

If you encounter any issues:

1. Make sure all scripts are executable:
   ```bash
   chmod +x build.sh install.sh create-deb.sh
   ```

2. Ensure you have all required dependencies:
   ```bash
   sudo apt install libgtkmm-3.0-dev libnotify-dev libayatana-appindicator3-dev libsqlite3-dev build-essential
   ```

3. Check the BUILD_FIX.md file for additional troubleshooting steps
