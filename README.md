# Hospital Management System

A comprehensive hospital management system built in C with GTK 3.0 graphical user interface.

## ✨ Features Implemented

### 🔐 Authentication & Security
- ✅ Secure login system with password hashing
- ✅ Role-based access control (Admin, Doctor, Nurse, Receptionist, Pharmacist)
- ✅ Session management
- ✅ Default admin account: `admin` / `admin123`

### 👥 Patient Management
- ✅ Complete patient registration form with validation
- ✅ Patient list view with all details
- ✅ Blood group and emergency contact management
- ✅ Medical history tracking (backend ready)
- ✅ Search and filter capabilities

### 👨‍⚕️ Doctor Management
- ✅ Doctor registration with 10 specializations
- ✅ Qualification and experience tracking
- ✅ Consultation fee management
- ✅ Doctor list with complete information
- ✅ Availability status

### 📅 Appointment System
- ✅ Book appointments with patient/doctor selection
- ✅ Date and time scheduling
- ✅ Conflict detection (prevents double-booking)
- ✅ Appointment status tracking
- ✅ Today's appointments view
- ✅ Reason for visit tracking

### 📊 Admin Dashboard
- ✅ Real-time statistics display
- ✅ Total patients, doctors, appointments count
- ✅ Pending appointments tracking
- ✅ Active users count
- ✅ User management interface

## 🚀 Quick Start

### Prerequisites

**Windows (MSYS2):**
```bash
# Install MSYS2 from https://www.msys2.org/
# Then in MSYS2 MinGW 64-bit terminal:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config make
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install build-essential libgtk-3-dev pkg-config
```

### Compilation

```bash
# In MSYS2 MinGW 64-bit terminal (Windows) or regular terminal (Linux)
cd /path/to/hospital_management
make
```

### Running

```bash
./hospital_management      # Linux
./hospital_management.exe  # Windows
```

### Default Login
- **Username:** `admin`
- **Password:** `admin123`

## 📖 User Guide

### Adding a Patient
1. Click "Patients" in sidebar
2. Click "➕ Add Patient"
3. Fill in all required fields:
   - Full Name (required)
   - Age (1-150)
   - Gender
   - Blood Group
   - Phone (validated)
   - Email (optional)
   - Address
   - Emergency Contact
4. Click "💾 Save Patient"

### Adding a Doctor
1. Click "Doctors" in sidebar
2. Click "➕ Add Doctor"
3. Enter doctor information:
   - Name
   - Specialization (10 options)
   - Qualifications (e.g., MBBS, MD)
   - Experience in years
   - Phone number
   - Email
   - Consultation Fee
4. Click "💾 Save Doctor"

### Booking an Appointment
1. Click "Appointments" in sidebar
2. Click "➕ Book Appointment"
3. Select patient from dropdown
4. Select doctor from dropdown
5. Enter date (DD/MM/YYYY format)
6. Enter time (HH:MM format)
7. Enter reason for visit
8. Click "📅 Book Appointment"
9. System automatically checks for conflicts

### Managing Users (Admin Only)
1. Click "Users" in sidebar (only visible to admins)
2. View all system users
3. Add new users with role assignment

## 🗂️ Project Structure

```
hospital_management/
├── Core System
│   ├── main.c              - Application entry point
│   ├── auth.c/h            - Authentication system
│   └── utils.c/h           - Utility functions
│
├── Patient Module
│   ├── patient.c/h         - Patient backend
│   └── patient_gui.c       - Patient GUI
│
├── Doctor Module
│   ├── doctor.c/h          - Doctor backend
│   └── doctor_gui.c        - Doctor GUI
│
├── Appointment Module
│   ├── appointment.c/h     - Appointment backend
│   └── appointment_gui.c   - Appointment GUI
│
├── Admin Panel
│   └── admin_panel.c/h     - Dashboard & user management
│
└── Configuration
    ├── Makefile            - Build configuration
    ├── compile.sh          - Helper script
    └── README.md           - This file
```

## 💾 Data Storage

All data is stored in binary files in the `data/` directory:
- `users.dat` - User accounts with hashed passwords
- `patients.dat` - Patient records
- `doctors.dat` - Doctor profiles
- `appointments.dat` - Appointment records

## 🎨 UI Features

- Modern GTK 3.0 interface
- Sidebar navigation
- Form validation
- Dialog boxes for all operations
- Emoji icons for better UX
- Responsive layouts
- Error handling with user-friendly messages

## 🔧 Troubleshooting

**"make: command not found"**
- Make sure you're using MSYS2 MinGW 64-bit terminal on Windows

**"Package gtk+-3.0 was not found"**
- Install GTK: `pacman -S mingw-w64-x86_64-gtk3`

**Application crashes on login**
- This has been fixed in the latest version
- Recompile with `make`

**Can't add patient/doctor**
- Ensure all required fields are filled
- Phone number must be 10-15 digits
- Age must be between 1-150

## 📝 User Roles

1. **Administrator** - Full system access, user management
2. **Doctor** - View patients, manage appointments
3. **Nurse** - Patient registration, basic management
4. **Receptionist** - Appointments, patient registration
5. **Pharmacist** - Pharmacy access (planned)

## 🔜 Future Enhancements

- Patient details/edit dialog
- Medical history viewer
- Appointment rescheduling
- Billing system
- Pharmacy inventory
- Staff management
- Report generation (PDF/CSV)
- Email/SMS notifications

## 📄 License

Educational project for learning C and GTK programming.

## 🤝 Support

For issues:
1. Check the troubleshooting section
2. Ensure GTK is properly installed
3. Verify you're using MSYS2 MinGW 64-bit terminal on Windows

---

**Status:** ✅ Fully functional with core features implemented!
