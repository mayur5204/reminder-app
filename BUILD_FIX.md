# Build and Installation Guide

## Issue Fix: Undefined References to ReminderPopupWindow

If you encounter linker errors like the following:

```
undefined reference to `ReminderPopupWindow::hide()'
undefined reference to `ReminderPopupWindow::signal_reminder_toggled()'
undefined reference to `ReminderPopupWindow::set_reminders(std::vector<Reminder, std::allocator<Reminder> > const&)'
undefined reference to `ReminderPopupWindow::show()'
undefined reference to `ReminderPopupWindow::ReminderPopupWindow()'
```

This issue has been fixed by updating the build process to directly compile and link all source files without relying on CMake.

## Simplified Build Process

We've simplified the build process to make it more reliable:

```bash
# Build the application
./build.sh

# Install the application locally
./install.sh

# Create a Debian package
./create-deb.sh
```

## What Changed

1. **Removed CMake dependency**:
   - The build process now directly uses g++ to compile and link the source files
   - All CMake-related files and configuration have been removed

2. **Simplified build scripts**:
   - Single `build.sh` script that handles all compilation
   - Single `install.sh` script for local installation
   - Single `create-deb.sh` script for creating a Debian package

3. **Fixed visual issues**:
   - Removed problematic "always on top" property which was causing build errors
   - Enhanced visual styling and checkboxes for the reminder popup window

## Troubleshooting

If you still encounter build issues, please try:

1. Ensure you have all required dependencies:
   ```bash
   sudo apt install libgtkmm-3.0-dev libnotify-dev libayatana-appindicator3-dev libsqlite3-dev build-essential
   ```

2. Check file permissions:
   ```bash
   chmod +x build.sh install.sh create-deb.sh
   ```

### 2. Modified CMake Method

If you prefer using CMake, make sure that:

1. The `src/reminder_popup_window.cpp` file is properly listed in the `SOURCES` variable in CMakeLists.txt
2. You're using the latest CMakeLists.txt with the diagnostic improvements

```bash
mkdir -p build
cd build
cmake ..
make
```

## Testing the Application

After successfully building the application, you can run it:

```bash
# If using the direct build method
./build-direct/reminder

# If using CMake
./build/reminder

# If installed
reminder
```

## Features Added in This Update

1. **Improved Popup Window UI**:
   - Better window styling and organization with titles and separators
   - Enhanced visual presentation of reminders

2. **Reminder Completion in Popup Window**:
   - Added checkboxes for marking reminders complete directly from the popup
   - Visual indicators for reminder status (completed, notified)
   - Strikethrough text for completed reminders

3. **Better Time Display**:
   - Conversion to 12-hour format with AM/PM indicator
   - Icon for time display

## Troubleshooting

If you still encounter build issues, please try:

1. Clean your build directory completely:
   ```bash
   rm -rf build build-direct
   ```

2. Ensure you have all required dependencies:
   ```bash
   sudo apt install libgtkmm-3.0-dev libnotify-dev libayatana-appindicator3-dev libsqlite3-dev build-essential cmake
   ```

3. Check file permissions:
   ```bash
   chmod -R u+rw .
   ```

4. Use the direct build method as a fallback.
