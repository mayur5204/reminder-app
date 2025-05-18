#!/bin/bash
# Script to prepare and push the repository to GitHub
# Usage: ./push-to-github.sh <github-username> <repository-name>

set -e

# Define color codes for better output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo -e "${RED}Git is not installed. Installing...${NC}"
    sudo apt update && sudo apt install -y git
fi

# Check if parameters are provided
if [ $# -ne 2 ]; then
    echo -e "${RED}Error: Missing parameters${NC}"
    echo "Usage: ./push-to-github.sh <github-username> <repository-name>"
    exit 1
fi

USERNAME=$1
REPO_NAME=$2
REPO_URL="https://github.com/$USERNAME/$REPO_NAME.git"

echo -e "${YELLOW}Preparing repository for GitHub...${NC}"

# Check if .git directory exists
if [ -d ".git" ]; then
    echo -e "${YELLOW}Git repository already initialized.${NC}"
else
    echo -e "${YELLOW}Initializing git repository...${NC}"
    git init
    # Rename the default branch to main
    git branch -m master main
fi

# Check if there are any uncommitted changes
if [ -n "$(git status --porcelain)" ]; then
    echo -e "${YELLOW}Adding files to git...${NC}"
    git add .
    
    echo -e "${YELLOW}Committing changes...${NC}"
    git commit -m "Initial commit for Snap Store deployment"
fi

# Check if the remote already exists
if git remote | grep -q "origin"; then
    echo -e "${YELLOW}Remote 'origin' already exists. Updating URL...${NC}"
    git remote set-url origin $REPO_URL
else
    echo -e "${YELLOW}Adding remote 'origin'...${NC}"
    git remote add origin $REPO_URL
fi

echo -e "${YELLOW}Pushing to GitHub...${NC}"
echo -e "${GREEN}Repository URL: $REPO_URL${NC}"
echo -e "${YELLOW}You'll need to enter your GitHub credentials to push.${NC}"
echo -e "${YELLOW}IMPORTANT: Make sure you've created the repository on GitHub first!${NC}"

# Get current branch name
CURRENT_BRANCH=$(git branch --show-current)
echo -e "${YELLOW}Current branch is: $CURRENT_BRANCH${NC}"

# Push to GitHub
git push -u origin $CURRENT_BRANCH

echo -e "${GREEN}Repository pushed to GitHub successfully!${NC}"
echo -e "${YELLOW}Next steps:${NC}"
echo "1. Go to https://snapcraft.io/publisher/builds"
echo "2. Click 'Import from GitHub'"
echo "3. Select your repository"
echo "4. Continue with the Snap Store integration as described in SNAP_PUBLISHING.md"
