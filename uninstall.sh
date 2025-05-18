#!/bin/bash
# Uninstall script for the Reminder application
# This script removes both local and system-wide installations

echo "Reminder Application Uninstaller"
echo "================================"
echo "This script will remove all installations of the Reminder application."
echo

# Define color codes for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to check if a command was successful
check_success() {
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Success${NC}"
    else
        echo -e "${RED}✗ Failed${NC}"
    fi
}

# Kill any running instances of the application
echo -n "Killing any running reminder processes... "
pkill -f reminder || true
check_success

# Function to remove local installation
remove_local_installation() {
    echo -e "\n${YELLOW}Removing local installation...${NC}"
    
    # Stop and disable user service if active
    echo -n "Stopping and disabling systemd user service... "
    systemctl --user stop reminder.service 2>/dev/null
    systemctl --user disable reminder.service 2>/dev/null
    check_success
    
    # Remove executable
    echo -n "Removing executable... "
    rm -f ~/.local/bin/reminder
    check_success
    
    # Remove desktop entries
    echo -n "Removing desktop entries... "
    rm -f ~/.local/share/applications/reminder.desktop
    rm -f ~/.config/autostart/reminder-autostart.desktop
    check_success
    
    # Remove icons
    echo -n "Removing icons... "
    rm -f ~/.local/share/icons/hicolor/128x128/apps/reminder.png
    rm -f ~/.local/share/pixmaps/reminder.png
    check_success
    
    # Remove systemd service file
    echo -n "Removing systemd service file... "
    rm -f ~/.config/systemd/user/reminder.service
    check_success
    
    # Reload systemd user daemon
    echo -n "Reloading systemd user daemon... "
    systemctl --user daemon-reload
    check_success
    
    echo -e "${GREEN}Local installation removed successfully.${NC}"
}

# Function to remove system-wide installation (Debian package)
remove_system_installation() {
    echo -e "\n${YELLOW}Removing system-wide installation...${NC}"
    
    # Check if the package is installed
    if dpkg -l | grep -q "^ii.*reminder"; then
        echo -n "Removing Debian package... "
        sudo dpkg -r reminder
        check_success
        
        # Clean package configuration if needed
        echo -n "Purging any remaining configuration... "
        sudo dpkg --purge reminder 2>/dev/null
        check_success
        
        echo -e "${GREEN}System-wide installation removed successfully.${NC}"
    else
        echo -e "${YELLOW}No system-wide installation found.${NC}"
    fi
}

# Remove database and settings
remove_data() {
    echo -e "\n${YELLOW}Removing application data...${NC}"
    
    # Ask user if they want to remove data
    read -p "Do you want to remove all application data including your reminders? (y/n): " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -n "Removing database file... "
        rm -f ~/.local/share/reminders.db
        check_success
        
        echo -e "${GREEN}Application data removed.${NC}"
    else
        echo -e "${YELLOW}Keeping application data.${NC}"
        echo "Your reminder data is preserved at: ~/.local/share/reminders.db"
    fi
}

# Main script execution
echo "Detecting installations..."

# Check for local installation
if [ -f ~/.local/bin/reminder ] || [ -f ~/.local/share/applications/reminder.desktop ]; then
    echo -e "${GREEN}Found local installation${NC}"
    remove_local_installation
else
    echo -e "${YELLOW}No local installation found${NC}"
fi

# Check for system-wide installation
if dpkg -l | grep -q "^ii.*reminder"; then
    echo -e "${GREEN}Found system-wide installation (Debian package)${NC}"
    remove_system_installation
else
    echo -e "${YELLOW}No system-wide installation (Debian package) found${NC}"
fi

# Handle application data
remove_data

echo -e "\n${GREEN}Uninstallation complete!${NC}"
echo "If you want to reinstall the application, you can use:"
echo "  - For local installation: ./install.sh"
echo "  - For system-wide installation: sudo dpkg -i reminder_1.0-1_amd64.deb"
