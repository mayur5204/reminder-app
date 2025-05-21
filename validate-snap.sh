#!/bin/bash
# Script to validate snapcraft.yaml for Snap Store submission

# Define color codes for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}Snap Store Submission Validator${NC}"
echo "=========================================="

VALIDATION_PASSED=true

# Check if snapcraft is installed
if ! command -v snapcraft &> /dev/null; then
    echo -e "${YELLOW}Warning: Snapcraft is not installed. Some validations will be skipped.${NC}"
fi

# Function to check if a file exists
check_file() {
    local file=$1
    local description=$2
    
    echo -n "Checking $description... "
    if [ -f "$file" ]; then
        echo -e "${GREEN}Found${NC}"
    else
        echo -e "${RED}Not found${NC}"
        VALIDATION_PASSED=false
    fi
}

# Validate required files
echo -e "\n${YELLOW}Step 1: Validating required files${NC}"
check_file "snap/snapcraft.yaml" "snapcraft.yaml"
check_file "snap/gui/reminder.desktop" "desktop file"
check_file "snap/gui/reminder-app.png" "icon file"

# Validate snapcraft.yaml content
echo -e "\n${YELLOW}Step 2: Validating snapcraft.yaml content${NC}"

echo -n "Checking if name matches the naming convention... "
if grep -q "^name: reminder-app$" snap/snapcraft.yaml; then
    echo -e "${GREEN}Valid${NC}"
else
    echo -e "${RED}Invalid${NC}"
    echo "The snap name should be 'reminder-app'."
    VALIDATION_PASSED=false
fi

echo -n "Checking if autostart is correctly configured... "
if grep -q "autostart: reminder.desktop" snap/snapcraft.yaml; then
    echo -e "${GREEN}Valid${NC}"
else
    echo -e "${YELLOW}Warning: Autostart might not be correctly configured.${NC}"
fi

echo -n "Checking if architectures are correctly specified... "
if grep -q "architectures:" snap/snapcraft.yaml && 
   grep -q "build-on: \[amd64\]" snap/snapcraft.yaml; then
    echo -e "${GREEN}Valid${NC}"
else
    echo -e "${YELLOW}Warning: Architectures might not be correctly specified.${NC}"
fi

# Validate desktop file content
echo -e "\n${YELLOW}Step 3: Validating desktop file content${NC}"

echo -n "Checking if desktop file has correct Exec line... "
if grep -q "^Exec=reminder-app$" snap/gui/reminder.desktop; then
    echo -e "${GREEN}Valid${NC}"
else
    echo -e "${YELLOW}Warning: Exec line should be 'Exec=reminder-app'${NC}"
fi

echo -n "Checking if desktop file has correct Icon line... "
if grep -q "Icon=.*reminder" snap/gui/reminder.desktop; then
    echo -e "${GREEN}Valid${NC}"
else
    echo -e "${YELLOW}Warning: Icon path might need to be adjusted${NC}"
fi

# Summary
echo -e "\n${YELLOW}Validation Summary${NC}"
if [ "$VALIDATION_PASSED" = true ]; then
    echo -e "${GREEN}All critical checks passed!${NC}"
    echo "Your snap package appears to be ready for submission to the Snap Store."
    echo "Follow the instructions in SNAP_PUBLISHING.md to publish your snap."
else
    echo -e "${RED}Some validation checks failed.${NC}"
    echo "Please fix the issues highlighted above before submitting to the Snap Store."
fi

echo -e "\n${BLUE}Next Steps${NC}"
echo "1. Commit and push any changes to GitHub"
echo "2. Connect your GitHub repository to Snapcraft Build Service"
echo "3. Follow the steps in SNAP_PUBLISHING.md to publish your snap"
