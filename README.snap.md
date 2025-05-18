# Reminder App - Snap Package

This document explains how to build, install, and publish the Reminder App as a Snap package.

## Building the Snap Package

To build the Snap package locally:

```bash
# Make sure you have snapcraft installed
sudo apt update
sudo apt install snapcraft

# Build the snap package
cd /path/to/reminder
snapcraft
```

If building on a non-Linux machine or if you encounter dependency issues, you can use multipass:

```bash
snapcraft --use-lxd
```

This will create a file named `reminder_1.0_amd64.snap`.

## Testing Locally

Before publishing, you can test your snap package:

```bash
# Install the snap locally
sudo snap install --dangerous ./reminder_1.0_amd64.snap

# Run the app
reminder

# Check the status of the service
snap services reminder
```

## Publishing to the Snap Store

1. Create an account on [Snapcraft.io](https://snapcraft.io)
2. Login from the terminal:
   ```bash
   snapcraft login
   ```
3. Register your app name:
   ```bash
   snapcraft register reminder
   ```
4. Upload your snap:
   ```bash
   snapcraft upload --release=stable reminder_1.0_amd64.snap
   ```
5. Complete your store listing at [https://snapcraft.io/publisher/](https://snapcraft.io/publisher/)

## Uninstalling the Snap

To uninstall:

```bash
sudo snap remove reminder
```

## Troubleshooting

If you encounter issues with the Snap package:

1. Check the service status:
   ```bash
   snap services reminder
   ```

2. View logs:
   ```bash
   snap logs reminder
   ```

3. If you have permission issues:
   ```bash
   snap connect reminder:your-interface-name
   ```

Note: The systemd service provided within the snap will run in the snap's namespace, not in the user's systemd service manager.
