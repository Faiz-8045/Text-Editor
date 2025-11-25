A simple File Editor made using C++ and VCPKG and build using CMAKE.
using the simple Guide from https://www.fltk.org/doc-1.1/editor.html .

AI has been used to Write the remaining Readme file and Comments. and Also with Cmake and VCPKG.

# Text Editor

A simple text editor built with FLTK (Fast Light Toolkit) based on the official FLTK 1.1 documentation.

## Features

- **File Operations**
  - New file (Ctrl+N)
  - Open file (Ctrl+O)
  - Insert file (Ctrl+I)
  - Save file (Ctrl+S)
  - Save As (Ctrl+Shift+S)
  - Exit (Ctrl+Q)

- **Edit Operations**
  - Undo (Ctrl+Z)
  - Cut (Ctrl+X)
  - Copy (Ctrl+C)
  - Paste (Ctrl+V)
  - Delete

- **Search Operations**
  - Find (Ctrl+F)
  - Find Again (Ctrl+G)
  - Replace (Ctrl+H)
  - Replace Again (Ctrl+T)
  - Replace All

- **Additional Features**
  - Change tracking (shows "modified" in title)
  - Prompt to save before closing unsaved changes
  - Monospaced font for code editing
  - Resizable window

## Prerequisites

You need to have FLTK library installed on your system.

### Installing FLTK on Windows

1. **Using vcpkg** (recommended):
   ```powershell
   vcpkg install fltk
   ```

2. **Manual installation**:
   - Download FLTK from https://www.fltk.org/software.php
   - Follow the build instructions in the FLTK documentation

### Installing FLTK on Linux

```bash
# Ubuntu/Debian
sudo apt-get install libfltk1.3-dev

# Fedora
sudo dnf install fltk-devel

# Arch Linux
sudo pacman -S fltk
```

### Installing FLTK on macOS

```bash
brew install fltk
```

## Building the Editor

### Using CMake

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
cmake --build .

# Run the editor
./editor
```

### Using fltk-config (Linux/macOS)

```bash
fltk-config --compile editor.cpp
./editor
```

### Manual compilation

```bash
# Linux/macOS
g++ -o editor editor.cpp `fltk-config --cxxflags --ldflags`

# Windows (with MSVC)
cl /EHsc editor.cpp /I"C:\path\to\fltk\include" /link fltk.lib
```

## Usage

1. Launch the editor:
   ```bash
   ./editor [filename]
   ```

2. If you provide a filename as an argument, it will be loaded automatically.

3. Use the menu bar or keyboard shortcuts to access all features.

4. The window title shows the current filename and modification status.

## Project Structure

```
SimpleTextEditor/
├── editor.cpp          # Main source file
├── CMakeLists.txt      # CMake build configuration
└── README.md           # This file
```

## Notes

- The editor uses a monospaced font (FL_COURIER) for better code editing.
- Unsaved changes are tracked and you'll be prompted before closing.
- The replace dialog is non-modal, allowing you to interact with the editor while it's open.

## License

This is a demonstration project based on the FLTK documentation example.

## References

- FLTK Documentation: https://www.fltk.org/doc-1.1/editor.html
- FLTK Official Website: https://www.fltk.org/
