# Hospital Management System - Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0` -lm

# Source files
SOURCES = main.c auth.c utils.c patient.c patient_gui.c doctor.c doctor_gui.c \
          appointment.c appointment_gui.c admission.c admission_gui.c \
          admin_panel.c doctor_panel.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
TARGET = hospital_management

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful! Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf data/*.dat
	@echo "Clean complete"

# Clean only object files
clean-obj:
	rm -f $(OBJECTS)
	@echo "Object files removed"

# Run the application
run: $(TARGET)
	./$(TARGET)

# Install GTK dependencies (for reference)
install-deps:
	@echo "On Ubuntu/Debian: sudo apt-get install libgtk-3-dev pkg-config"
	@echo "On Fedora: sudo dnf install gtk3-devel pkg-config"
	@echo "On Windows (MSYS2): pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config"

.PHONY: all clean clean-obj run install-deps
