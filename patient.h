#ifndef PATIENT_H
#define PATIENT_H

#include "utils.h"

typedef enum {
    BLOOD_A_POS, BLOOD_A_NEG, BLOOD_B_POS, BLOOD_B_NEG,
    BLOOD_AB_POS, BLOOD_AB_NEG, BLOOD_O_POS, BLOOD_O_NEG
} BloodGroup;

typedef enum {
    GENDER_MALE, GENDER_FEMALE, GENDER_OTHER
} Gender;

typedef struct {
    Date visit_date;
    char diagnosis[MAX_TEXT];
    char treatment[MAX_TEXT];
    char doctor_name[MAX_STRING];
    double cost;
} MedicalRecord;

typedef struct {
    int id;
    char name[MAX_STRING];
    int age;
    Gender gender;
    BloodGroup blood_group;
    char phone[MAX_PHONE];
    char email[MAX_EMAIL];
    char address[MAX_TEXT];
    char emergency_contact[MAX_PHONE];
    char emergency_contact_name[MAX_STRING];
    Date registration_date;
    int is_active;
    int primary_doctor_id;  // Assigned primary care physician
    MedicalRecord medical_history[50];
    int medical_record_count;
} Patient;

void init_patient_system();
int add_patient(Patient* patient);
int update_patient(Patient* patient);
int delete_patient(int patient_id);
Patient* get_patient_by_id(int patient_id);
Patient* search_patients_by_name(const char* name, int* count);
Patient* search_patients_by_phone(const char* phone);
int get_all_patients(Patient** patients);
int add_medical_record(int patient_id, MedicalRecord* record);
const char* get_blood_group_name(BloodGroup bg);
const char* get_gender_name(Gender gender);
void save_patients_to_file();
void load_patients_from_file();
int assign_patient_to_doctor(int patient_id, int doctor_id);
int get_patients_by_doctor(int doctor_id, Patient** patients);
GtkWidget* create_patient_list_view(GtkWidget* parent_window);
void show_add_patient_dialog(GtkWidget* parent_window);
void show_assign_doctor_dialog(GtkWidget* parent_window, int patient_id);

#endif
