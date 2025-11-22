#include "patient.h"
#include <string.h>

#define MAX_PATIENTS 1000
#define PATIENTS_FILE "data/patients.dat"

static Patient patients[MAX_PATIENTS];
static int patient_count = 0;

void init_patient_system() { load_patients_from_file(); }

int add_patient(Patient* patient) {
    if (patient_count >= MAX_PATIENTS) return 0;
    patient->id = patient_count + 1;
    get_current_date(&patient->registration_date);
    patient->is_active = 1;
    patient->medical_record_count = 0;
    if (patient->primary_doctor_id == 0) {
        patient->primary_doctor_id = 0; // No doctor assigned initially
    }
    patients[patient_count++] = *patient;
    save_patients_to_file();
    return 1;
}

int update_patient(Patient* patient) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == patient->id) {
            patient->registration_date = patients[i].registration_date;
            patient->medical_record_count = patients[i].medical_record_count;
            memcpy(patient->medical_history, patients[i].medical_history, sizeof(MedicalRecord) * patients[i].medical_record_count);
            patients[i] = *patient;
            save_patients_to_file();
            return 1;
        }
    }
    return 0;
}

int delete_patient(int patient_id) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == patient_id) {
            patients[i].is_active = 0;
            save_patients_to_file();
            return 1;
        }
    }
    return 0;
}

Patient* get_patient_by_id(int patient_id) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == patient_id && patients[i].is_active) return &patients[i];
    }
    return NULL;
}

Patient* search_patients_by_name(const char* name, int* count) {
    static Patient results[MAX_PATIENTS];
    *count = 0;
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].is_active && strstr(patients[i].name, name) != NULL) {
            results[(*count)++] = patients[i];
        }
    }
    return results;
}

Patient* search_patients_by_phone(const char* phone) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].is_active && strcmp(patients[i].phone, phone) == 0) return &patients[i];
    }
    return NULL;
}

int get_all_patients(Patient** patients_ptr) {
    static Patient active_patients[MAX_PATIENTS];
    int count = 0;
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].is_active) active_patients[count++] = patients[i];
    }
    *patients_ptr = active_patients;
    return count;
}

int add_medical_record(int patient_id, MedicalRecord* record) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == patient_id && patients[i].is_active) {
            if (patients[i].medical_record_count >= 50) return 0;
            patients[i].medical_history[patients[i].medical_record_count++] = *record;
            save_patients_to_file();
            return 1;
        }
    }
    return 0;
}

const char* get_blood_group_name(BloodGroup bg) {
    switch (bg) {
        case BLOOD_A_POS: return "A+";
        case BLOOD_A_NEG: return "A-";
        case BLOOD_B_POS: return "B+";
        case BLOOD_B_NEG: return "B-";
        case BLOOD_AB_POS: return "AB+";
        case BLOOD_AB_NEG: return "AB-";
        case BLOOD_O_POS: return "O+";
        case BLOOD_O_NEG: return "O-";
        default: return "Unknown";
    }
}

const char* get_gender_name(Gender gender) {
    switch (gender) {
        case GENDER_MALE: return "Male";
        case GENDER_FEMALE: return "Female";
        case GENDER_OTHER: return "Other";
        default: return "Unknown";
    }
}

void save_patients_to_file() {
    FILE* file = fopen(PATIENTS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(PATIENTS_FILE, "wb");
        if (!file) { printf("Error: Cannot create patients file\n"); return; }
    }
    fwrite(&patient_count, sizeof(int), 1, file);
    fwrite(patients, sizeof(Patient), patient_count, file);
    fclose(file);
}

void load_patients_from_file() {
    FILE* file = fopen(PATIENTS_FILE, "rb");
    if (!file) { patient_count = 0; return; }
    fread(&patient_count, sizeof(int), 1, file);
    fread(patients, sizeof(Patient), patient_count, file);
    fclose(file);
}

int assign_patient_to_doctor(int patient_id, int doctor_id) {
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].id == patient_id && patients[i].is_active) {
            patients[i].primary_doctor_id = doctor_id;
            save_patients_to_file();
            return 1;
        }
    }
    return 0;
}

int get_patients_by_doctor(int doctor_id, Patient** patients_ptr) {
    static Patient doctor_patients[MAX_PATIENTS];
    int count = 0;
    for (int i = 0; i < patient_count; i++) {
        if (patients[i].is_active && patients[i].primary_doctor_id == doctor_id) {
            doctor_patients[count++] = patients[i];
        }
    }
    *patients_ptr = doctor_patients;
    return count;
}
