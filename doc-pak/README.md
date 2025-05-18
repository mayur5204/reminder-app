# Reminder App

A native Ubuntu desktop reminder application written in C++ using GTKmm.

## Features

- Add, edit, and delete reminders with title, description, and time
- Desktop notifications at specified times
- Mark reminders as completed
- Persistent storage using SQLite database

## Dependencies

- gtkmm-3.0
- libnotify
- sqlite3
- cmake
- make
- g++

## Building from Source

1. Install the dependencies:

```bash
sudo apt-get update
sudo apt-get install libgtkmm-3.0-dev libnotify-dev libsqlite3-dev cmake build-essential
```

2. Clone the repository:

```bash
git clone https://github.com/yourusername/reminder.git
cd reminder
```

3. Create a build directory and build the application:

```bash
mkdir build
cd build
cmake ..
make
```

4. Run the application:

```bash
./reminder
```

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
5. When a reminder time is reached, a desktop notification will be displayed.

## License

This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.
