#!/bin/bash
# Script to deeply clean the repository before pushing to GitHub

# Don't exit on error
set +e

# Define color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}Deep Repository Cleaning Tool for Reminder App${NC}"
echo "=================================================="

# Step 1: Remove build directories and temporary files
echo -e "\n${YELLOW}Step 1: Removing build directories and temporary files${NC}"

echo -n "Removing build directories and artifacts... "
# Forcefully remove all build directories and artifacts
sudo rm -rf parts/ prime/ stage/ build/ .snapcraft/ 2>/dev/null || true
rm -rf parts/ prime/ stage/ build/ .snapcraft/ 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo -n "Removing any compiled binaries... "
rm -f reminder 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo -n "Removing any snap packages... "
rm -f *.snap 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

# Step 2: Clean up duplicate and redundant documentation
echo -e "\n${YELLOW}Step 2: Organizing documentation${NC}"

echo -n "Creating documentation directory... "
mkdir -p docs 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo -n "Moving specialized documentation to docs directory... "
# Only move files that aren't core documentation
for doc in AUTOSTART_FIX.md IMPLEMENTATION.md TROUBLESHOOTING.md UNINSTALL_FIX.md; do
    if [ -f "$doc" ]; then
        mv "$doc" docs/ 2>/dev/null || true
    fi
done
echo -e "${GREEN}Done${NC}"

# Step 3: Clean up redundant scripts
echo -e "\n${YELLOW}Step 3: Organizing scripts${NC}"

echo -n "Creating scripts directory... "
mkdir -p scripts 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo -n "Moving utility scripts to scripts directory... "
# Move utility scripts that aren't main build/install scripts
cp prepare-for-github.sh scripts/ 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

# Step 4: Additional cleanup for problematic directories
echo -e "\n${YELLOW}Step 4: Performing additional cleanup${NC}"

if [ -d "parts" ]; then
    echo -n "Parts directory still exists, attempting stronger removal... "
    # Try with sudo if available
    if command -v sudo &> /dev/null; then
        sudo rm -rf parts/
    else
        # Try with different permissions
        chmod -R +w parts/ 2>/dev/null
        rm -rf parts/
    fi
    
    if [ -d "parts" ]; then
        echo -e "${RED}Failed${NC} - Please manually remove the parts directory"
    else
        echo -e "${GREEN}Success${NC}"
    fi
fi

echo -n "Removing any VS Code settings... "
rm -rf .vscode/ 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

echo -n "Removing any temporary or hidden files... "
find . -type f -name "*.bak" -o -name "*.tmp" -o -name "*~" -o -name ".DS_Store" -delete 2>/dev/null || true
echo -e "${GREEN}Done${NC}"

# Step 5: Verification and summary
echo -e "\n${YELLOW}Step 5: Verifying repository structure${NC}"

echo "Current workspace structure:"
find . -maxdepth 2 -type d | sort
echo ""
echo "Documentation files:"
find . -name "*.md" | sort
echo ""
echo "Scripts:"
find . -name "*.sh" | sort

echo -e "\n${GREEN}Deep cleaning complete!${NC}"

# Final verification
if [ -d "parts" ] || [ -d "prime" ] || [ -d "stage" ]; then
    echo -e "\n${RED}WARNING:${NC} Some build directories still exist!"
    echo "You may need to manually remove the following directories:"
    [ -d "parts" ] && echo "  - parts/"
    [ -d "prime" ] && echo "  - prime/"
    [ -d "stage" ] && echo "  - stage/"
    echo -e "\nTry running: sudo rm -rf parts/ prime/ stage/"
else
    echo -e "\n${GREEN}Repository successfully cleaned!${NC}"
fi

echo 
echo "Your repository is now ready for GitHub submission."

# Check if Git repository already exists
if [ -d ".git" ]; then
    echo "Git repository is already initialized. To push changes:"
    echo "  git add ."
    echo "  git commit -m \"Clean repository for Snap Store submission\""
    echo "  git push origin main"
else
    echo "Run the following commands to initialize Git and push to GitHub:"
    echo "  git init"
    echo "  git add ."
    echo "  git commit -m \"Initial commit for Snap Store submission\""
    echo "  git remote add origin https://github.com/<your-username>/reminder-app.git"
    echo "  git branch -M main"
    echo "  git push -u origin main"
fi

echo
echo "After pushing to GitHub, connect to the Snap Store as described in SNAP_PUBLISHING.md"
