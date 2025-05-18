# Troubleshooting Guide

## Common Issues and Solutions

### Command Line Errors

1. **Issue**: `GLib-GIO-CRITICAL **: This application can not open files.`
   - **Solution**: Make sure you're using the correct syntax for the command-line options. 
     - Correct: `reminder --minimize` (not `reminder -- --minimize`)

2. **Issue**: Application doesn't start in the system tray
   - **Solution**: 
     - Make sure you've installed the `libayatana-appindicator3-1` package
     - Check that your desktop environment supports AppIndicators
     - Try manually running with `reminder --minimize` to see any error messages

### System Tray Issues

1. **Issue**: System tray icon doesn't appear
   - **Solution**:
     - Ensure your desktop environment supports AppIndicator
     - Try running `sudo apt install libayatana-appindicator3-1` to ensure dependencies are installed
     - Restart your desktop environment/session

2. **Issue**: Icon appears but is a generic icon
   - **Solution**: 
     - Make sure the icon file was properly installed to either `/usr/share/icons/hicolor/128x128/apps/reminder.png` or `/usr/share/pixmaps/reminder.png`
     - Try reinstalling with `./install.sh`

### Notification Issues

1. **Issue**: Not receiving notifications
   - **Solution**:
     - Make sure notifications are enabled in your system settings
     - Ensure the application is running in the background (check with `ps aux | grep reminder`)
     - Check if the time formatting matches your system (the app uses 24-hour format internally)

2. **Issue**: Receiving duplicate notifications for the same reminder
   - **Solution**:
     - This should be fixed in the latest version, which tracks which reminders have already been notified
     - If you're upgrading from an older version, make sure to rebuild/reinstall the application
     - The database will be automatically updated to include the new tracking field

3. **Issue**: Notifications don't appear the next day for daily reminders
   - **Solution**:
     - The app now resets notification status at midnight
     - Make sure the app is running continuously for this to work
     - If the app was restarted, it will reset notification status on startup

### Autostart Issues

1. **Issue**: Application doesn't start on login
   - **Solution**:
     - Check if the autostart file is properly installed: `ls -la ~/.config/autostart/reminder-autostart.desktop`
     - Make sure the file has the correct path to the executable
     - Ensure the X-GNOME-Autostart-enabled flag is set to true
     - Try enabling the systemd service: `systemctl --user enable reminder.service`

### Database Issues

1. **Issue**: Reminders not being saved or loaded
   - **Solution**:
     - Check if the database exists: `ls -la ~/.local/share/reminders.db`
     - Try checking permissions: `chmod 644 ~/.local/share/reminders.db`
     - If database is corrupted, you might need to delete it: `rm ~/.local/share/reminders.db` (warning: will lose data)

## Getting Help

If you encounter issues not covered in this guide, please:
1. Check the terminal output for any error messages
2. File an issue on the GitHub repository with details about:
   - Your Ubuntu version
   - Desktop environment
   - Steps to reproduce the issue
   - Any error messages
