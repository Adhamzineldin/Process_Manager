# Process Manager

A Linux process management utility with both CLI and GUI interfaces.

## Features

The Process Manager provides the following capabilities:

- List all processes in the system
- List processes grouped by user
- Display process IDs of all processes
- Run/stop specific processes
- Send specific signals to specific processes

## Requirements

### CLI Version
- GCC compiler
- Standard C libraries

### GUI Version
- GTK+ 3.0 development libraries

## Installation

### Install Dependencies

For Debian/Ubuntu systems:
```bash
sudo apt-get update
sudo apt-get install build-essential libgtk-3-dev
```

For Red Hat/Fedora systems:
```bash
sudo dnf install gcc make gtk3-devel
```

### Build and Install

1. Clone or download this repository
2. Navigate to the project directory
3. Run the installation script:
```bash
sudo ./install.sh
```

Alternatively, you can build manually:
```bash
make clean
make all
sudo make install
```

## Usage

### Command Line Interface

Run the CLI version:
```bash
procmanager
```

### Graphical User Interface

Run the GUI version:
```bash
procmanager-gui
```

### Man Page

View the manual page:
```bash
man procmanager
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.