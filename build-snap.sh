#!/bin/bash
# Script to build and publish the Reminder app as a Snap package

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

echo -e "${YELLOW}Reminder App - Snap Package Builder${NC}"
echo "===============================\n"

# Check if snapcraft is installed
echo -n "Checking if snapcraft is installed... "
if ! command -v snapcraft &> /dev/null; then
    echo -e "${RED}✗ Not found${NC}"
    echo "Installing snapcraft..."
    echo "Snapcraft requires classic confinement..."
    sudo snap install snapcraft --classic
    check_success
else
    echo -e "${GREEN}✓ Found${NC}"
fi

# Build the snap package
echo -e "\n${YELLOW}Building Snap package...${NC}"
echo "This may take a while and require downloading dependencies."
echo "Building using multipass for improved compatibility..."
echo -n "Building... "
snapcraft --use-lxd
check_success

SNAP_FILE=$(ls -1t *.snap | head -1)
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
    echo "You can now test the application by running: reminder"
    echo "Check the service status with: snap services reminder"
fi

# Ask if user wants to publish to Snap Store
echo
read -p "Do you want to publish this Snap to the Snap Store? (y/n): " -n 1 -r PUBLISH
echo
if [[ $PUBLISH =~ ^[Yy]$ ]]; then
    echo -e "\n${YELLOW}Publishing to Snap Store...${NC}"
    
    # Login to Snapcraft
    echo "First, you need to log in to your Snapcraft account."
    echo "If you don't have an account, visit https://snapcraft.io and create one."
    echo -n "Logging in... "
    snapcraft login
    check_success
    
    # Register the snap name
    echo -e "\n${YELLOW}Registering the snap name 'reminder'...${NC}"
    echo "If this name is already taken, the command will fail and you'll need to choose another name."
    echo "In that case, you'll need to update the name in snap/snapcraft.yaml and rebuild."
    echo -n "Registering... "
    snapcraft register reminder || true
    
    # Upload to the store
    echo -e "\n${YELLOW}Uploading snap to the store...${NC}"
    echo -n "Uploading... "
    snapcraft upload --release=stable $SNAP_FILE
    check_success
    
    echo -e "\n${GREEN}Snap uploaded successfully!${NC}"
    echo "Complete your store listing by visiting: https://snapcraft.io/publisher/snaps"
    echo "Make sure to add a good description, screenshots, and icon."
fi

echo -e "\n${GREEN}All done!${NC}"
echo "Your Snap package is ready at: $SNAP_FILE"
echo "For more information, see README.snap.md"
