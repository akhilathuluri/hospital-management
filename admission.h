#ifndef ADMISSION_H
#define ADMISSION_H

#include "utils.h"

typedef enum {
    WARD_GENERAL,
    WARD_ICU,
    WARD_EMERGENCY,
    WARD_PEDIATRIC,
    WARD_MATERNITY
} WardType;

typedef enum {
    BED_AVAILABLE,
    BED_OCCUPIED,
    BED_MAINTENANCE
} BedStatus;

typedef enum {
    ADMISSION_ACTIVE,
    ADMISSION_DISCHARGED,
    ADMISSION_TRANSFERRED
} AdmissionStatus;

typedef struct {
    int id;
    int bed_number;
    WardType ward;
    BedStatus status;
    int current_patient_id;
} Bed;

typedef struct {
    int id;
    int patient_id;
    int doctor_id;
    int bed_id;
    Date admission_date;
    Time admission_time;
    Date expected_discharge_date;
    Date actual_discharge_date;
    Time discharge_time;
    char admission_reason[MAX_TEXT];
    char diagnosis[MAX_TEXT];
    char discharge_summary[MAX_TEXT];
    AdmissionStatus status;
    double total_charges;
    int is_active;
} Admission;

// Bed management
void init_admission_system();
void init_beds();
int get_available_beds_by_ward(WardType ward, Bed** beds);
int get_all_beds(Bed** beds);
Bed* get_bed_by_id(int bed_id);
int update_bed_status(int bed_id, BedStatus status);

// Admission management
int admit_patient(Admission* admission);
int discharge_patient(int admission_id, const char* discharge_summary, double charges);
int transfer_patient(int admission_id, int new_bed_id);
Admission* get_admission_by_id(int admission_id);
int get_active_admissions(Admission** admissions);
int get_admissions_by_patient(int patient_id, Admission** admissions);
int get_admissions_by_doctor(int doctor_id, Admission** admissions);
int calculate_length_of_stay(Admission* admission);

// Utility functions
const char* get_ward_name(WardType ward);
const char* get_bed_status_name(BedStatus status);
const char* get_admission_status_name(AdmissionStatus status);

// File persistence
void save_admissions_to_file();
void load_admissions_from_file();
void save_beds_to_file();
void load_beds_from_file();

// GUI functions
GtkWidget* create_admission_view(GtkWidget* parent_window);
void show_admit_patient_dialog(GtkWidget* parent_window);
void show_discharge_dialog(GtkWidget* parent_window, int admission_id);

#endif
