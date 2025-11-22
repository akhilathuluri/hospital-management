#ifndef DOCTOR_PANEL_H
#define DOCTOR_PANEL_H

#include "utils.h"
#include "auth.h"

typedef struct {
    int total_my_patients;
    int appointments_today;
    int admitted_patients;
    int completed_appointments;
} DoctorStats;

DoctorStats get_doctor_statistics(int doctor_id);
GtkWidget* create_doctor_dashboard(GtkWidget* parent_window);
GtkWidget* create_doctor_patients_view(GtkWidget* parent_window);
GtkWidget* create_doctor_appointments_view(GtkWidget* parent_window);
GtkWidget* create_doctor_admissions_view(GtkWidget* parent_window);

#endif
