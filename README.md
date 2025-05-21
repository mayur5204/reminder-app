# Reminder App

A native Ubuntu desktop reminder application written in C++ using GTKmm.

## Features

- Add, edit, and delete reminders with title, description, and time
- Desktop notifications at specified times
- Mark reminders as completed
- Persistent storage using SQLite database
- System tray integration with Ayatana AppIndicator
- Autostart on system boot
- Systemd user service for reliable background operation
- Enhanced popup window with checkboxes for marking reminders
- Improved visual styling and display

## Dependencies

- gtkmm-3.0
- libnotify
- sqlite3
- libayatana-appindicator3
- g++ (build only)

## Building and Installation

### Prerequisites

Install the required dependencies:

```bash
sudo apt-get update
sudo apt-get install libgtkmm-3.0-dev libnotify-dev libsqlite3-dev libayatana-appindicator3-dev build-essential
```

### Building

To build the application, simply run:

```bash
./build.sh
```

This will create the executable at `build/reminder`.

### Installation

There are several ways to install the Reminder App:

#### Method 1: Using the Install Script (Local Installation)

```bash
sudo apt install libgtkmm-3.0-dev libnotify-dev libsqlite3-dev libayatana-appindicator3-dev

# Clone the repository
git clone https://github.com/mayur5204/reminder-app.git
cd reminder-app

# Build and install
./build.sh
./install.sh
```

#### Method 2: Using the Debian Package

To create a Debian package:

```bash
./create-deb.sh
```

Then install the package:

```bash
sudo dpkg -i reminder_1.0-1_amd64.deb
```

```bash
./reminder
```

#### Method 3: Using Snap (Recommended)

The application is available as a snap package for easy installation across Ubuntu and other Linux distributions:

```bash
sudo snap install reminder-app
```

If you prefer to build the snap package yourself:

```bash
# Install snapcraft
sudo snap install snapcraft --classic

# Build the snap package
snapcraft

# Install the built package
sudo snap install --dangerous ./reminder-app_1.0_amd64.snap
```

For detailed information about Snap deployment, see [SNAP_PUBLISHING.md](SNAP_PUBLISHING.md).

## Easy Installation

For a quick local installation that adds the application to your Applications menu:

```bash
./build.sh  # Build the application
./install.sh  # Install it locally
```

This will:
- Copy the executable to `~/.local/bin/`
- Create a desktop entry in `~/.local/share/applications/`
- Install the icon to `~/.local/share/icons/`

## Creating a Debian Package

### Method 1: Using CPack (CMake packaging)

1. Install the required packaging tools:

```bash
sudo apt-get install devscripts debhelper dh-make
```

2. Build the Debian package:

```bash
cd /path/to/reminder
mkdir -p build-deb
cd build-deb
cmake ..
make
cpack -G DEB
```

3. Install the generated .deb package:

```bash
sudo dpkg -i reminder-1.0-Linux.deb
```

### Method 2: Using Debian packaging tools

```bash
cd /path/to/reminder
dpkg-buildpackage -us -uc -b
```

The .deb package will be created in the parent directory.

### Method 3: Using checkinstall (Easiest)

For a simpler approach, you can use the provided script:

```bash
./create-deb.sh
```

This uses `checkinstall` to create and install a .deb package in one step.

## Usage

1. Launch the application from the applications menu or by running `reminder` in the terminal.
2. Enter a title, description, and time for your reminder.
3. Click "Add Reminder" to create a new reminder.
4. You can edit or delete reminders using the corresponding buttons.
5. To start the application minimized in the system tray, use the `--minimize` or `-m` flag:
   ```bash
   reminder --minimize
   ```
5. The application will continue running in the system tray when you close the window.
6. You'll receive desktop notifications when a reminder is due.

### Command-line Options:

- `--minimize` or `-m`: Start the application minimized to the system tray (default behavior)
- `--show` or `-s`: Start with the main window visible (overrides the default minimized behavior)

## System Tray Integration

The application integrates with the system tray (using Ayatana AppIndicator) to provide:

- Minimized operation - the app runs in the background when closed
- Quick access to view your reminders in a compact popup window
- Option to add new reminders via the main application window
- Easy way to quit the application completely

When clicking "Show Reminders" in the system tray menu, a small draggable popup window appears displaying all your reminders. This provides a quick overview without opening the full application.

To add or edit reminders, use the "Add New Reminder" option which opens the full application window.

## Autostart and Background Operation

The application can:

1. Start automatically when you log in (via XDG autostart)
2. Run reliably as a systemd user service that restarts if it crashes
3. Start minimized to the system tray with the `--minimize` command-line option:
   ```bash
   reminder --minimize
   ```

## Uninstallation

To remove the application from your system:

```bash
./uninstall.sh
```

This script will:
- Remove both local and system-wide installations if present
- Clean up desktop entries, icons, and systemd services
- Give you the option to keep or remove your reminder data

If you installed from a .deb package:

```bash
sudo apt remove reminder
```
5. When a reminder time is reached, a desktop notification will be displayed.

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

## Install from Snap Store

The Reminder App is available on the Snap Store. To install:

```bash
sudo snap install reminder-app
```

After installation, you can launch the app from your application menu or by running:

```bash
reminder-app
```
