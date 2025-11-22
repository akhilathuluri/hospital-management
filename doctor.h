#ifndef DOCTOR_H
#define DOCTOR_H

#include "utils.h"

typedef enum {
    SPEC_GENERAL, SPEC_CARDIOLOGY, SPEC_NEUROLOGY, SPEC_ORTHOPEDICS,
    SPEC_PEDIATRICS, SPEC_GYNECOLOGY, SPEC_DERMATOLOGY, SPEC_ENT,
    SPEC_OPHTHALMOLOGY, SPEC_PSYCHIATRY
} Specialization;

typedef struct {
    int id;
    char name[MAX_STRING];
    Specialization specialization;
    char qualifications[MAX_STRING];
    int experience_years;
    char phone[MAX_PHONE];
    char email[MAX_EMAIL];
    double consultation_fee;
    int is_available;
    int is_active;
    Date joining_date;
} Doctor;

void init_doctor_system();
int add_doctor(Doctor* doctor);
int update_doctor(Doctor* doctor);
int delete_doctor(int doctor_id);
Doctor* get_doctor_by_id(int doctor_id);
int get_all_doctors(Doctor** doctors);
int get_available_doctors(Doctor** doctors);
const char* get_specialization_name(Specialization spec);
void save_doctors_to_file();
void load_doctors_from_file();
GtkWidget* create_doctor_list_view(GtkWidget* parent_window);
void show_add_doctor_dialog(GtkWidget* parent_window);

#endif
