# Publishing to the Snap Store

This document provides instructions for publishing the Reminder App to the Snap Store.

## Method 1: Using GitHub and the Snap Store Build Service

The easiest way to publish to the Snap Store is to use GitHub integration:

### Step 1: Create a GitHub Repository

```bash
# Navigate to your project
cd ~/Documents/Projects/reminder

# Initialize git repository (if not already done)
git init

# Add all files
git add .

# Commit the changes
git commit -m "Initial commit for Snap Store"

# Create a repository on GitHub from the website, then:
git remote add origin https://github.com/mayur5204/reminder-app.git
git push -u origin main
```

### Step 2: Connect to the Snap Store

1. **Create a Snap Store account** (if you don't have one):
   - Go to [https://snapcraft.io/](https://snapcraft.io/)
   - Click "Sign up"
   - Follow the registration process

2. **Log in to Snapcraft from terminal**:
   ```bash
   snapcraft login
   ```
   Enter your Snap Store credentials when prompted.

3. **Register your snap name**:
   ```bash
   snapcraft register reminder-app
   ```
   If "reminder-app" is taken, try alternatives like "reminder-tool" or "mayur-reminder".

4. **Connect your GitHub repository to the Snap Store**:
   - Go to [https://snapcraft.io/publisher/builds](https://snapcraft.io/publisher/builds)
   - Click "Import from GitHub"
   - Select your repository
   - Select "reminder-app" as the name (or whatever name you registered)
   - Choose the branch (main or master)
   - Click "Import"

### Step 3: Monitor and Publish Your Snap

1. **Wait for the build to complete** on the Snap Store dashboard.
2. **When the build is finished**:
   - Go to [https://snapcraft.io/publisher/snaps](https://snapcraft.io/publisher/snaps)
   - Select your snap
   - Click "Release"
   - Choose the channel (usually "stable")
   - Add release notes
   - Click "Release"

3. **Complete your store listing**:
   - Add application screenshots
   - Complete the description
   - Add any additional metadata

## Method 2: Building Locally and Pushing Manually

If you prefer to build locally and push manually:

### Step 1: Build the Snap Package

```bash
# Navigate to your project
cd ~/Documents/Projects/reminder

# Build using our provided script (requires LXD setup)
./build-snap.sh

# Or build directly:
snapcraft --use-lxd
```

### Step 2: Publish Manually

```bash
# Login to Snapcraft
snapcraft login

# Register your snap name (if not already done)
snapcraft register reminder-app

# Release to the store
snapcraft upload --release=stable ./reminder-app_1.0_amd64.snap
```

## Testing Your Published Snap

After publishing, users can install your snap with:

```bash
sudo snap install reminder-app
```

## Updating Your Snap

To update your snap:

1. Make changes to your code
2. Push to GitHub (for Method 1) or build locally (for Method 2)
3. The new version will either be built automatically (Method 1) or you'll need to manually upload it (Method 2)
4. Release the new version through the Snap Store dashboard

## Troubleshooting

If you encounter issues:

- **Build errors**: Check the build logs in the Snap Store dashboard
- **Permission issues**: Make sure your snap has the correct plugs/slots defined
- **Icon not showing**: Ensure you have provided a proper icon in the snap/gui/ directory
- **Service not starting**: Check if your daemon configuration is correct
