#include "appointment.h"

#define MAX_APPOINTMENTS 5000
#define APPOINTMENTS_FILE "data/appointments.dat"

static Appointment appointments[MAX_APPOINTMENTS];
static int appointment_count = 0;

void init_appointment_system() { load_appointments_from_file(); }

int book_appointment(Appointment* appointment) {
    if (appointment_count >= MAX_APPOINTMENTS) return 0;
    if (check_appointment_conflict(appointment->doctor_id, appointment->appointment_date, appointment->appointment_time)) return 0;
    appointment->id = appointment_count + 1;
    get_current_date(&appointment->created_date);
    appointment->status = APPT_PENDING;
    appointments[appointment_count++] = *appointment;
    save_appointments_to_file();
    return 1;
}

int update_appointment(Appointment* appointment) {
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].id == appointment->id) {
            appointment->created_date = appointments[i].created_date;
            appointments[i] = *appointment;
            save_appointments_to_file();
            return 1;
        }
    }
    return 0;
}

int cancel_appointment(int appointment_id) {
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].id == appointment_id) {
            appointments[i].status = APPT_CANCELLED;
            save_appointments_to_file();
            return 1;
        }
    }
    return 0;
}

Appointment* get_appointment_by_id(int appointment_id) {
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].id == appointment_id) return &appointments[i];
    }
    return NULL;
}

int get_appointments_by_patient(int patient_id, Appointment** appointments_ptr) {
    static Appointment patient_appointments[MAX_APPOINTMENTS];
    int count = 0;
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].patient_id == patient_id && appointments[i].status != APPT_CANCELLED) {
            patient_appointments[count++] = appointments[i];
        }
    }
    *appointments_ptr = patient_appointments;
    return count;
}

int get_appointments_by_doctor(int doctor_id, Appointment** appointments_ptr) {
    static Appointment doctor_appointments[MAX_APPOINTMENTS];
    int count = 0;
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].doctor_id == doctor_id && appointments[i].status != APPT_CANCELLED) {
            doctor_appointments[count++] = appointments[i];
        }
    }
    *appointments_ptr = doctor_appointments;
    return count;
}

int get_appointments_by_date(Date date, Appointment** appointments_ptr) {
    static Appointment date_appointments[MAX_APPOINTMENTS];
    int count = 0;
    for (int i = 0; i < appointment_count; i++) {
        if (compare_dates(appointments[i].appointment_date, date) == 0 && appointments[i].status != APPT_CANCELLED) {
            date_appointments[count++] = appointments[i];
        }
    }
    *appointments_ptr = date_appointments;
    return count;
}

int get_all_appointments(Appointment** appointments_ptr) {
    static Appointment all_appointments[MAX_APPOINTMENTS];
    int count = 0;
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].status != APPT_CANCELLED) {
            all_appointments[count++] = appointments[i];
        }
    }
    *appointments_ptr = all_appointments;
    return count;
}

int check_appointment_conflict(int doctor_id, Date date, Time time) {
    for (int i = 0; i < appointment_count; i++) {
        if (appointments[i].doctor_id == doctor_id &&
            compare_dates(appointments[i].appointment_date, date) == 0 &&
            appointments[i].appointment_time.hour == time.hour &&
            appointments[i].appointment_time.minute == time.minute &&
            appointments[i].status != APPT_CANCELLED) {
            return 1;
        }
    }
    return 0;
}

const char* get_appointment_status_name(AppointmentStatus status) {
    switch (status) {
        case APPT_PENDING: return "Pending";
        case APPT_CONFIRMED: return "Confirmed";
        case APPT_COMPLETED: return "Completed";
        case APPT_CANCELLED: return "Cancelled";
        default: return "Unknown";
    }
}

void save_appointments_to_file() {
    FILE* file = fopen(APPOINTMENTS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(APPOINTMENTS_FILE, "wb");
        if (!file) return;
    }
    fwrite(&appointment_count, sizeof(int), 1, file);
    fwrite(appointments, sizeof(Appointment), appointment_count, file);
    fclose(file);
}

void load_appointments_from_file() {
    FILE* file = fopen(APPOINTMENTS_FILE, "rb");
    if (!file) { appointment_count = 0; return; }
    fread(&appointment_count, sizeof(int), 1, file);
    fread(appointments, sizeof(Appointment), appointment_count, file);
    fclose(file);
}
