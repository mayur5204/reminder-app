#!/bin/bash
# Script to create a GitHub repository and push the code
# Usage: ./github-setup.sh <github-username> <repository-name> <github-token>

set -e

# Define color codes for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Check if parameters are provided
if [ $# -ne 3 ]; then
    echo -e "${RED}Error: Missing parameters${NC}"
    echo "Usage: ./github-setup.sh <github-username> <repository-name> <github-token>"
    echo ""
    echo "To create a GitHub token:"
    echo "1. Go to https://github.com/settings/tokens"
    echo "2. Click 'Generate new token'"
    echo "3. Select 'repo' and 'workflow' scopes"
    echo "4. Click 'Generate token'"
    exit 1
fi

USERNAME=$1
REPO_NAME=$2
TOKEN=$3
REPO_URL="https://github.com/$USERNAME/$REPO_NAME.git"

echo -e "${YELLOW}Creating repository on GitHub...${NC}"

# Create the repository on GitHub
response=$(curl -s -X POST \
  -H "Authorization: token $TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/user/repos \
  -d "{\"name\":\"$REPO_NAME\",\"description\":\"Reminder App - Ubuntu desktop notification application\",\"private\":false}")

# Check if repository was created
if echo "$response" | grep -q "errors"; then
    echo -e "${RED}Failed to create repository:${NC}"
    echo "$response" | grep -o '"message": "[^"]*"'
    echo "If the repository already exists, you can proceed with the push."
else
    echo -e "${GREEN}Repository created successfully!${NC}"
fi

echo -e "${YELLOW}Preparing local repository...${NC}"

# Initialize git if needed
if [ ! -d ".git" ]; then
    echo -e "${YELLOW}Initializing git repository...${NC}"
    git init
    git branch -m master main
fi

# Add all files
echo -e "${YELLOW}Adding files to git...${NC}"
git add .

# Commit if there are changes
if git status | grep -q "Changes to be committed"; then
    echo -e "${YELLOW}Committing changes...${NC}"
    git commit -m "Initial commit for Snap Store deployment"
fi

# Add/update remote
if git remote | grep -q "origin"; then
    echo -e "${YELLOW}Updating remote URL...${NC}"
    git remote set-url origin https://${USERNAME}:${TOKEN}@github.com/${USERNAME}/${REPO_NAME}.git
else
    echo -e "${YELLOW}Adding remote...${NC}"
    git remote add origin https://${USERNAME}:${TOKEN}@github.com/${USERNAME}/${REPO_NAME}.git
fi

# Push to GitHub
echo -e "${YELLOW}Pushing to GitHub...${NC}"
CURRENT_BRANCH=$(git branch --show-current)
git push -u origin $CURRENT_BRANCH

echo -e "${GREEN}Repository pushed to GitHub successfully!${NC}"
echo -e "${YELLOW}Next steps:${NC}"
echo "1. Go to https://snapcraft.io/publisher/builds"
echo "2. Click 'Import from GitHub'"
echo "3. Select your repository ($REPO_NAME)"
echo "4. Continue with the Snap Store integration as described in SNAP_PUBLISHING.md"
