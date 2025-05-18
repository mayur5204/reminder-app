#!/bin/bash
set -e  # Exit on error

# Script to create a Debian package for the Reminder application

echo "Creating Debian package for Reminder application..."

# First build the application
./build.sh

# Check if build was successful
if [ ! -f "build/reminder" ]; then
    echo "Error: Build failed. Cannot create Debian package."
    exit 1
fi

# Clean up any previous packaging attempts
echo "Cleaning up previous packaging artifacts..."
rm -rf debian/reminder
rm -f reminder_*.deb

# Create necessary directories
mkdir -p debian/reminder/usr/bin
mkdir -p debian/reminder/usr/share/applications
mkdir -p debian/reminder/usr/share/icons/hicolor/128x128/apps
mkdir -p debian/reminder/usr/share/pixmaps
# mkdir -p debian/reminder/etc/xdg/autostart
mkdir -p debian/reminder/usr/lib/systemd/user
mkdir -p debian/reminder/DEBIAN

# Copy files to the package structure
cp build/reminder debian/reminder/usr/bin/
cp data/reminder.desktop debian/reminder/usr/share/applications/
# cp data/reminder-autostart.desktop debian/reminder/etc/xdg/autostart/
cp data/reminder.png debian/reminder/usr/share/icons/hicolor/128x128/apps/
cp data/reminder.png debian/reminder/usr/share/pixmaps/
cp data/reminder.service debian/reminder/usr/lib/systemd/user/

# Set permissions
chmod 755 debian/reminder/usr/bin/reminder
chmod 644 debian/reminder/usr/share/applications/reminder.desktop
# chmod 644 debian/reminder/etc/xdg/autostart/reminder-autostart.desktop
chmod 644 debian/reminder/usr/share/icons/hicolor/128x128/apps/reminder.png
chmod 644 debian/reminder/usr/share/pixmaps/reminder.png
chmod 644 debian/reminder/usr/lib/systemd/user/reminder.service

# Create control file
cat > debian/reminder/DEBIAN/control << 'CONTROLEOF'
Package: reminder
Version: 1.0-1
Section: utils
Priority: optional
Architecture: amd64
Depends: libgtkmm-3.0-1v5, libnotify4, libsqlite3-0, libayatana-appindicator3-1
Maintainer: Your Name <your.email@example.com>
Description: A simple reminder application with system tray integration
 A desktop application to manage your daily reminders with notifications,
 system tray integration, and autostart capability.
CONTROLEOF

# Create a post-installation script to set up the desktop database
cat > debian/reminder/DEBIAN/postinst << 'POSTINSTEOF'
#!/bin/bash
set -e
if [ "$1" = "configure" ]; then
    update-desktop-database -q || true
    gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
exit 0
POSTINSTEOF

# Make the postinst script executable
chmod 755 debian/reminder/DEBIAN/postinst

# Create the package
echo "Building Debian package..."
dpkg-deb --build debian/reminder .

# Verify the package was created
if [ -f reminder_1.0-1_amd64.deb ]; then
    echo "Debian package created: reminder_1.0-1_amd64.deb"
    echo "You can install it with: sudo dpkg -i reminder_1.0-1_amd64.deb"
else
    echo "Debian package creation failed!"
    exit 1
fi

# Cleanup temporary build files if requested
read -p "Do you want to clean up temporary build files? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Cleaning up temporary build files..."
    rm -rf debian/reminder
    echo "Cleanup complete."
fi
