#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include "utils.h"

typedef enum {
    APPT_PENDING, APPT_CONFIRMED, APPT_COMPLETED, APPT_CANCELLED
} AppointmentStatus;

typedef struct {
    int id;
    int patient_id;
    int doctor_id;
    Date appointment_date;
    Time appointment_time;
    AppointmentStatus status;
    char reason[MAX_TEXT];
    char notes[MAX_TEXT];
    Date created_date;
} Appointment;

void init_appointment_system();
int book_appointment(Appointment* appointment);
int update_appointment(Appointment* appointment);
int cancel_appointment(int appointment_id);
Appointment* get_appointment_by_id(int appointment_id);
int get_appointments_by_patient(int patient_id, Appointment** appointments);
int get_appointments_by_doctor(int doctor_id, Appointment** appointments);
int get_appointments_by_date(Date date, Appointment** appointments);
int get_all_appointments(Appointment** appointments);
int check_appointment_conflict(int doctor_id, Date date, Time time);
const char* get_appointment_status_name(AppointmentStatus status);
void save_appointments_to_file();
void load_appointments_from_file();
GtkWidget* create_appointment_view(GtkWidget* parent_window);
void show_book_appointment_dialog(GtkWidget* parent_window);

#endif
