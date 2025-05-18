#!/bin/bash
set -e  # Exit on error

# Script to install the Reminder application locally

echo "Installing Reminder application..."

# Check if build exists
if [ ! -f "build/reminder" ]; then
    echo "Building the application first..."
    ./build.sh
fi

# Create directories if they don't exist
mkdir -p $HOME/.local/bin
mkdir -p $HOME/.local/share/applications
mkdir -p $HOME/.local/share/icons/hicolor/128x128/apps
mkdir -p $HOME/.local/share/pixmaps
mkdir -p $HOME/.config/autostart
mkdir -p $HOME/.config/systemd/user

# Copy the executable
cp build/reminder $HOME/.local/bin/
chmod +x $HOME/.local/bin/reminder

# Copy the desktop file
cp data/reminder.desktop $HOME/.local/share/applications/
# Update the Exec path in the desktop file
sed -i "s|Exec=reminder|Exec=$HOME/.local/bin/reminder|g" $HOME/.local/share/applications/reminder.desktop

# Use systemd service for autostart (don't use desktop autostart)
# cp data/reminder-autostart.desktop $HOME/.config/autostart/
# sed -i "s|Exec=reminder|Exec=$HOME/.local/bin/reminder|g" $HOME/.config/autostart/reminder-autostart.desktop

# Copy icons
cp data/reminder.png $HOME/.local/share/icons/hicolor/128x128/apps/
cp data/reminder.png $HOME/.local/share/pixmaps/

# Copy systemd service file
cp data/reminder.service $HOME/.config/systemd/user/
# Update the ExecStart path in the service file
sed -i "s|ExecStart=/usr/bin/reminder|ExecStart=$HOME/.local/bin/reminder|g" $HOME/.config/systemd/user/reminder.service

# Enable the systemd service
systemctl --user daemon-reload
systemctl --user enable reminder.service

echo "Installation completed successfully!"
echo "You can now run the application with 'reminder' command"
echo "The application will also start automatically on system startup"

# Copy the desktop file
cat > $HOME/.local/share/applications/reminder.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Reminder
Comment=Simple reminder application
Exec=$HOME/.local/bin/reminder
Icon=reminder
Terminal=false
Categories=Utility;GTK;
Keywords=reminder;task;notification;
EOF

# We no longer use desktop autostart file, only systemd service
# Desktop autostart file was removed to prevent duplicate instances

# Install systemd user service
cat > $HOME/.config/systemd/user/reminder.service << EOF
[Unit]
Description=Reminder Application Service
After=graphical-session.target
PartOf=graphical-session.target

[Service]
Type=simple
ExecStart=$HOME/.local/bin/reminder --minimize
Restart=on-failure
RestartSec=5

[Install]
WantedBy=graphical-session.target
EOF

# Enable the systemd user service
systemctl --user daemon-reload
systemctl --user enable reminder.service

# Copy the icon to multiple locations for better compatibility
cp data/reminder.png $HOME/.local/share/icons/hicolor/128x128/apps/reminder.png
cp data/reminder.png $HOME/.local/share/pixmaps/reminder.png

echo "Installation complete. You can now find 'Reminder' in your applications menu."
echo "The app will also start automatically when you log in."
echo "You can also run it from the terminal with 'reminder'."

# Add the local bin directory to PATH if it's not already there
if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    echo "Adding $HOME/.local/bin to PATH in your .bashrc"
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> $HOME/.bashrc
    echo "Please restart your terminal or run 'source $HOME/.bashrc' to update your PATH."
fi
