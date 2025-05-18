# Autostart Configuration Fix

## Problem
The Reminder application was configured to start automatically in two different ways:

1. Using a desktop autostart file (`~/.config/autostart/reminder-autostart.desktop`)
2. Using a systemd user service (`~/.config/systemd/user/reminder.service`)

This was causing two instances of the application to run simultaneously, resulting in duplicate status bar icons.

## Solution

The application now exclusively uses **systemd user service** for autostart functionality. This provides better control over the application lifecycle and ensures only one instance runs at startup.

Changes made:
1. Removed the desktop autostart file (`reminder-autostart.desktop`)
2. Enhanced the systemd service file with proper process management and ExecStop command
3. Updated `main.cpp` to include instance checking to prevent multiple instances
4. Modified installation scripts to use only one autostart method
5. Improved the destructor in `reminder_app.cpp` for better status icon cleanup
6. Enhanced uninstall script to properly terminate running instances

## Checking the Current Autostart Configuration

To check if the systemd service is enabled:
```bash
systemctl --user status reminder.service
```

To enable/disable autostart:
```bash
# Enable
systemctl --user enable reminder.service

# Disable
systemctl --user disable reminder.service
```

## Troubleshooting

If you still see multiple instances:

1. Make sure no old autostart entries exist:
```bash
rm -f ~/.config/autostart/reminder-autostart.desktop
```

2. Make sure only one systemd service is running:
```bash
systemctl --user restart reminder.service
```

3. Check for running instances:
```bash
ps aux | grep reminder
```
