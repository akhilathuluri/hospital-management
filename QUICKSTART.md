# Hospital Management System - Quick Start Guide

## 🏥 What You Have

A complete **Hospital Management System** with graphical user interface built in C using GTK 3.0.

## 📦 Files Created (20 files)

### Source Code (19 files)
- **Core**: main.c, auth.c/h, utils.c/h
- **Patient Module**: patient.c/h, patient_gui.c
- **Doctor Module**: doctor.c/h, doctor_gui.c
- **Appointment Module**: appointment.c/h, appointment_gui.c
- **Admin Panel**: admin_panel.c/h
- **Config**: Makefile, styles.css
- **Docs**: README.md, INSTALL_WINDOWS.md

### Documentation
- README.md - Complete user manual
- INSTALL_WINDOWS.md - Windows installation guide
- walkthrough.md - Detailed implementation walkthrough

## 🚀 Quick Start

### Step 1: Install GTK (Windows)

**Option A: MSYS2 (Recommended)**
1. Download MSYS2: https://www.msys2.org/
2. Install to `C:\msys64`
3. Open **MSYS2 MinGW 64-bit** terminal
4. Run:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config make
```

### Step 2: Compile

In MSYS2 MinGW 64-bit terminal:
```bash
cd /e/Mohit-Project/hospital_management
make
```

### Step 3: Run

```bash
./hospital_management.exe
```

### Step 4: Login

- **Username**: `admin`
- **Password**: `admin123`

## ✨ Features

### ✅ Implemented
- 🔐 **Login System** - Secure authentication with 5 user roles
- 👥 **Patient Management** - Add, view, search patients
- 👨‍⚕️ **Doctor Management** - Register doctors with specializations
- 📅 **Appointments** - Book appointments with conflict detection
- 📊 **Admin Dashboard** - Real-time statistics and user management
- 💾 **Data Persistence** - File-based storage

### 🎨 UI Features
- Modern GTK 3.0 interface
- Sidebar navigation
- Form validation
- Search functionality
- Custom CSS styling

## 📁 Project Location

```
E:\Mohit-Project\hospital_management\
```

## 🔧 Build Commands

```bash
make          # Compile the project
make run      # Compile and run
make clean    # Remove build files
```

## 📖 Documentation

- **README.md** - Full user manual with features and usage
- **INSTALL_WINDOWS.md** - Detailed Windows installation guide
- **walkthrough.md** - Complete implementation details

## 🎯 User Roles

1. **Admin** - Full system access
2. **Doctor** - Medical staff access
3. **Nurse** - Nursing staff access
4. **Receptionist** - Front desk access
5. **Pharmacist** - Pharmacy access

## 💡 Next Steps

1. **Install GTK** using INSTALL_WINDOWS.md guide
2. **Compile** with `make`
3. **Run** the application
4. **Login** as admin
5. **Add sample data** to test features

## 🐛 Troubleshooting

**GTK not found?**
→ See INSTALL_WINDOWS.md for detailed setup

**Compilation errors?**
→ Ensure you're using MSYS2 MinGW 64-bit terminal

**Can't run executable?**
→ Add `C:\msys64\mingw64\bin` to Windows PATH

## 📞 Support

Refer to:
- README.md for usage instructions
- INSTALL_WINDOWS.md for installation help
- walkthrough.md for technical details

---

**Status**: ✅ **Ready to Compile and Run**

All source code is complete. Just install GTK and compile!
