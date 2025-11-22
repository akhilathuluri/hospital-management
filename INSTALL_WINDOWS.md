# GTK Installation Guide for Windows

## Quick Setup for Hospital Management System

This guide will help you install GTK 3.0 on Windows to compile and run the Hospital Management System.

## Method 1: MSYS2 (Recommended)

### Step 1: Download and Install MSYS2

1. Go to https://www.msys2.org/
2. Download the installer (msys2-x86_64-*.exe)
3. Run the installer
4. Install to: `C:\msys64` (default)
5. Click "Finish" and keep "Run MSYS2 now" checked

### Step 2: Update MSYS2

In the MSYS2 terminal that opens, run:

```bash
pacman -Syu
```

If it asks to close the terminal, close it and reopen **MSYS2 MSYS** from Start Menu, then run again:

```bash
pacman -Su
```

### Step 3: Install Development Tools

Close the current terminal and open **MSYS2 MinGW 64-bit** from Start Menu.

Run these commands:

```bash
# Install GCC compiler
pacman -S mingw-w64-x86_64-gcc

# Install GTK 3
pacman -S mingw-w64-x86_64-gtk3

# Install pkg-config
pacman -S mingw-w64-x86_64-pkg-config

# Install make
pacman -S make
```

Type `Y` when prompted to proceed with installation.

### Step 4: Verify Installation

```bash
# Check GCC
gcc --version

# Check pkg-config
pkg-config --version

# Check GTK
pkg-config --modversion gtk+-3.0
```

You should see version numbers for all three.

### Step 5: Navigate to Project and Compile

```bash
# Navigate to your project (adjust path as needed)
cd /e/Mohit-Project/hospital_management

# Compile
make

# Run
./hospital_management.exe
```

## Method 2: Pre-built GTK Bundle (Alternative)

### Step 1: Download GTK Bundle

1. Go to: https://github.com/tschoonj/GTK-for-Windows-Runtime-Environment-Installer/releases
2. Download the latest installer
3. Run and install to default location

### Step 2: Install MinGW-w64

1. Download from: https://sourceforge.net/projects/mingw-w64/
2. Install to `C:\mingw64`
3. Add `C:\mingw64\bin` to PATH

### Step 3: Set Environment Variables

Add these to your PATH:
- `C:\Program Files\GTK3-Runtime Win64\bin`
- `C:\mingw64\bin`

### Step 4: Compile Manually

Open Command Prompt in project directory:

```cmd
gcc -Wall -Wextra -IC:\path\to\gtk\include\gtk-3.0 -IC:\path\to\gtk\include\glib-2.0 -c *.c

gcc *.o -o hospital_management.exe -LC:\path\to\gtk\lib -lgtk-3 -lgdk-3 -lglib-2.0 -lgobject-2.0
```

## Troubleshooting

### Issue: "pacman: command not found"
**Solution**: Make sure you're using MSYS2 MinGW 64-bit terminal, not regular Command Prompt

### Issue: "Package gtk+-3.0 was not found"
**Solution**: 
1. Reinstall GTK: `pacman -S mingw-w64-x86_64-gtk3`
2. Check pkg-config: `pkg-config --list-all | grep gtk`

### Issue: "Cannot find -lgtk-3"
**Solution**: Make sure you're using MSYS2 MinGW 64-bit terminal, not MSYS2 MSYS

### Issue: DLL not found when running
**Solution**: Add `C:\msys64\mingw64\bin` to your Windows PATH

## Quick Test

Create a test file `test.c`:

```c
#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Test");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
```

Compile and run:

```bash
gcc test.c -o test `pkg-config --cflags --libs gtk+-3.0`
./test.exe
```

If a window appears, GTK is working!

## Running the Hospital Management System

Once GTK is installed:

```bash
# In MSYS2 MinGW 64-bit terminal
cd /e/Mohit-Project/hospital_management
make
./hospital_management.exe
```

Default login:
- Username: `admin`
- Password: `admin123`

## Additional Resources

- MSYS2 Documentation: https://www.msys2.org/docs/what-is-msys2/
- GTK Documentation: https://docs.gtk.org/gtk3/
- GTK Tutorial: https://www.gtk.org/docs/getting-started/

---

**Note**: The MSYS2 method is recommended as it provides a complete Unix-like environment with package management, making it easier to install and update dependencies.
