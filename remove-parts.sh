#!/bin/bash
# Script to forcefully remove the parts directory and other build artifacts

set -e

# Define color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Forceful removal of build directories${NC}"
echo "========================================"

echo -e "This script will attempt to remove all build artifacts with elevated permissions."
echo -e "You may be prompted for your password if sudo is required."

echo -n "Removing parts directory... "
if [ -d "parts" ]; then
    # Try to change permissions first
    chmod -R +w parts/ 2>/dev/null || true
    
    # Try normal removal
    rm -rf parts/ 2>/dev/null || true
    
    # If still exists, try with sudo
    if [ -d "parts" ] && command -v sudo &> /dev/null; then
        sudo rm -rf parts/
    fi
    
    if [ -d "parts" ]; then
        echo -e "${RED}Failed${NC}"
    else
        echo -e "${GREEN}Success${NC}"
    fi
else
    echo -e "${GREEN}Already removed${NC}"
fi

echo -n "Removing prime directory... "
if [ -d "prime" ]; then
    rm -rf prime/ 2>/dev/null || sudo rm -rf prime/ 2>/dev/null
    [ -d "prime" ] && echo -e "${RED}Failed${NC}" || echo -e "${GREEN}Success${NC}"
else
    echo -e "${GREEN}Already removed${NC}"
fi

echo -n "Removing stage directory... "
if [ -d "stage" ]; then
    rm -rf stage/ 2>/dev/null || sudo rm -rf stage/ 2>/dev/null
    [ -d "stage" ] && echo -e "${RED}Failed${NC}" || echo -e "${GREEN}Success${NC}"
else
    echo -e "${GREEN}Already removed${NC}"
fi

echo -n "Removing .snapcraft directory... "
if [ -d ".snapcraft" ]; then
    rm -rf .snapcraft/ 2>/dev/null || sudo rm -rf .snapcraft/ 2>/dev/null
    [ -d ".snapcraft" ] && echo -e "${RED}Failed${NC}" || echo -e "${GREEN}Success${NC}"
else
    echo -e "${GREEN}Already removed${NC}"
fi

echo -n "Removing snap build artifacts... "
rm -f *.snap 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo 
if [ -d "parts" ] || [ -d "prime" ] || [ -d "stage" ] || [ -d ".snapcraft" ]; then
    echo -e "${RED}Some directories could not be removed.${NC}"
    echo "You may need to manually delete them or run this as root."
    echo "After cleanup, run the clean-repository.sh script again."
else
    echo -e "${GREEN}All build artifacts successfully removed!${NC}"
    echo "Now run the clean-repository.sh script to complete the cleanup."
fi
