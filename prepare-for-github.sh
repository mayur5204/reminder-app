#!/bin/bash
# Script to prepare Reminder App for GitHub and Snap Store build service

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
        # Don't exit on error, just continue
    fi
}

echo -e "${YELLOW}Reminder App - GitHub & Snap Store Preparation (Safe Mode)${NC}"
echo "=====================================================\n"

# Clean up the workspace without removing directories
echo -e "${YELLOW}Step 1: Safely cleaning the workspace...${NC}"
echo -n "Removing temporary files... "

# Try to remove only user-owned files, ignoring permission errors
rm -f *.snap *.deb *.build *.changes *.buildinfo 2>/dev/null || true
rm -f reminder src/reminder_app.cpp.snap 2>/dev/null || true

# Clean snap directory without removing it
echo -n "Organizing snap directory... "
# Creates snap/local if it doesn't exist
mkdir -p snap/local 2>/dev/null || true
# Move any extra files to snap/local
if [ -f "snap/snapcraft.yaml.backup" ]; then
  mv snap/snapcraft.yaml.backup snap/local/ 2>/dev/null || true
fi
if [ -f "snap/snapcraft.yaml.simple" ]; then
  mv snap/snapcraft.yaml.simple snap/local/ 2>/dev/null || true
fi
check_success

# Set up .gitignore if it doesn't exist
echo -e "\n${YELLOW}Step 2: Setting up .gitignore...${NC}"
echo -n "Creating .gitignore... "
cat > .gitignore << EOF
# Build files
*.o
*.a
*.so
*.deb
*.snap
reminder
parts/
stage/
prime/
build/

# Editor files
*~
.*.swp
.vscode/
.idea/
*.bak

# Temporary files
*.log
.snapcraft/
*.snap
EOF
check_success

# Verify snapcraft.yaml is valid
echo -e "\n${YELLOW}Step 3: Verifying snapcraft.yaml...${NC}"
echo -n "Validating snapcraft.yaml... "
if [ -f "snap/snapcraft.yaml" ]; then
  # Basic validation - just check if it's parsable YAML
  # Don't fail if Python isn't available
  cat snap/snapcraft.yaml | python3 -c "import yaml, sys; yaml.safe_load(sys.stdin)" 2>/dev/null
  check_success
else
  echo -e "${RED}✗ Failed${NC}"
  echo "snap/snapcraft.yaml not found. Please ensure it exists."
fi

# Update README.md with Snap Store installation instructions
echo -e "\n${YELLOW}Step 4: Updating README with Snap installation instructions...${NC}"
echo -n "Updating README.md... "
if [ -f "README.md" ]; then
  # Check if Snap installation instructions already exist
  if ! grep -q "## Install from Snap Store" README.md; then
    # Append Snap installation instructions
    cat >> README.md << EOF

## Install from Snap Store

The Reminder App is available on the Snap Store. To install:

\`\`\`bash
sudo snap install reminder-app
\`\`\`

After installation, you can launch the app from your application menu or by running:

\`\`\`bash
reminder-app
\`\`\`
EOF
  fi
  check_success
else
  echo -e "${YELLOW}⚠ Warning${NC}: README.md not found. Creating a simple README.md file."
  cat > README.md << EOF
# Reminder App

A simple reminder application for Ubuntu with system tray integration.

## Features

* Create and manage reminders with customizable schedules
* System tray integration for easy access
* Desktop notifications when reminders are due
* Persistent storage of your reminders

## Install from Snap Store

The Reminder App is available on the Snap Store. To install:

\`\`\`bash
sudo snap install reminder-app
\`\`\`

After installation, you can launch the app from your application menu or by running:

\`\`\`bash
reminder-app
\`\`\`
EOF
  check_success
fi

echo -e "\n${GREEN}Preparation complete!${NC}"
echo 
echo "Next steps:"
echo "1. Initialize a Git repository (if not already done):"
echo "   git init"
echo 
echo "2. Add all files to Git:"
echo "   git add ."
echo 
echo "3. Commit your changes:"
echo "   git commit -m \"Prepare for Snap Store submission\""
echo
echo "4. Create a repository on GitHub"
echo "   Go to github.com and create a new repository named 'reminder-app'"
echo
echo "5. Push to GitHub:"
echo "   git remote add origin https://github.com/<your-username>/reminder-app.git"
echo "   git push -u origin main"
echo
echo "6. Connect to the Snap Store:"
echo "   - Go to snapcraft.io/publisher/builds"
echo "   - Click \"Import from GitHub\""
echo "   - Select your repository"
echo "   - Select \"reminder-app\" as the name"
echo
echo "For detailed instructions, refer to SNAP_PUBLISHING.md"
