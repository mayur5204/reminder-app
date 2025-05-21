# Snap Installation Guide for Reminder App

This guide will walk you through installing the Reminder App using Snap packages.

## Prerequisites

- Ubuntu 18.04 or later
- Snapd installed (comes pre-installed on Ubuntu)

## Installation Methods

### Method 1: Install from Snap Store (Recommended)

Once the snap package is approved and published, you can install it with:

```bash
sudo snap install reminder-app
```

### Method 2: Install from Local Build

If you have built the snap package locally:

1. Build the snap with the build script:
   ```bash
   ./fix-build-snap.sh
   ```

2. Install the generated snap:
   ```bash
   sudo snap install --dangerous ./reminder-app_1.0_amd64.snap
   ```

## Running the App

After installing, you can run the app in two ways:

1. From the application menu: Look for "Reminder" in your applications
2. From the command line:
   ```bash
   reminder-app
   ```

## Verifying Installation

To check if the snap service is running:

```bash
snap services reminder-app
```

To view logs:

```bash
snap logs reminder-app
```

## Troubleshooting

### Multiple Icons in Status Bar

If you see multiple icons in the status bar:

1. Check running instances:
   ```bash
   ps aux | grep reminder
   ```

2. Stop the snap service:
   ```bash
   snap stop reminder-app
   ```

3. Kill any remaining processes:
   ```bash
   pkill -f reminder
   ```

4. Restart the service:
   ```bash
   snap start reminder-app
   ```

### Permissions Issues

If the app doesn't have the right permissions:

```bash
snap connect reminder-app:home :home
snap connect reminder-app:desktop :desktop
```

### Complete Uninstallation

To completely remove the app:

```bash
snap stop reminder-app
snap remove reminder-app
```

## Feedback and Support

If you encounter any issues with the snap package, please report them at our GitHub repository.
