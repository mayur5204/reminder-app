#!/bin/bash
# Direct Snap Build script that uses destructive mode
# This builds directly on your system without virtualization

set -e

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
        exit 1
    fi
}

echo -e "${YELLOW}Reminder App - Direct Snap Builder${NC}"
echo "===================================\n"

# Check if snapcraft is installed
echo -n "Checking if snapcraft is installed... "
if ! command -v snapcraft &> /dev/null; then
    echo -e "${RED}✗ Not found${NC}"
    echo "Installing snapcraft..."
    sudo snap install snapcraft --classic
    check_success
else
    echo -e "${GREEN}✓ Found${NC}"
fi

# Clean any previous build artifacts
echo -n "Cleaning previous build artifacts... "
snapcraft clean
check_success

# Build the snap package
echo -e "\n${YELLOW}Building Snap package using destructive mode...${NC}"
echo "This will build directly on your system without using containers or VMs."
echo "Requires sudo access to create directories in /snap"
echo -n "Building... "
sudo snapcraft --destructive-mode
check_success

SNAP_FILE=$(ls -1t *.snap | head -1)
if [ -f "$SNAP_FILE" ]; then
    echo -e "\n${GREEN}Successfully built: ${SNAP_FILE}${NC}"

    # Ask if user wants to install and test locally
    echo
    read -p "Do you want to install and test the Snap locally? (y/n): " -n 1 -r TEST_LOCAL
    echo
    if [[ $TEST_LOCAL =~ ^[Yy]$ ]]; then
        echo -e "\n${YELLOW}Installing Snap locally for testing...${NC}"
        echo -n "Installing... "
        sudo snap install --dangerous ./$SNAP_FILE
        check_success
        
        echo -e "\n${GREEN}Snap installed successfully!${NC}"
        echo "You can now test the application by running: reminder-app"
        echo "Check the service status with: snap services reminder-app"
    fi
else
    echo -e "\n${RED}Build failed - no snap file was created.${NC}"
fi

echo -e "\n${GREEN}All done!${NC}"
