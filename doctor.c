#include "doctor.h"

#define MAX_DOCTORS 200
#define DOCTORS_FILE "data/doctors.dat"

static Doctor doctors[MAX_DOCTORS];
static int doctor_count = 0;

void init_doctor_system() { load_doctors_from_file(); }

int add_doctor(Doctor* doctor) {
    if (doctor_count >= MAX_DOCTORS) return 0;
    doctor->id = doctor_count + 1;
    get_current_date(&doctor->joining_date);
    doctor->is_active = 1;
    doctor->is_available = 1;
    doctors[doctor_count++] = *doctor;
    save_doctors_to_file();
    return 1;
}

int update_doctor(Doctor* doctor) {
    for (int i = 0; i < doctor_count; i++) {
        if (doctors[i].id == doctor->id) {
            doctor->joining_date = doctors[i].joining_date;
            doctors[i] = *doctor;
            save_doctors_to_file();
            return 1;
        }
    }
    return 0;
}

int delete_doctor(int doctor_id) {
    for (int i = 0; i < doctor_count; i++) {
        if (doctors[i].id == doctor_id) {
            doctors[i].is_active = 0;
            save_doctors_to_file();
            return 1;
        }
    }
    return 0;
}

Doctor* get_doctor_by_id(int doctor_id) {
    for (int i = 0; i < doctor_count; i++) {
        if (doctors[i].id == doctor_id && doctors[i].is_active) return &doctors[i];
    }
    return NULL;
}

int get_all_doctors(Doctor** doctors_ptr) {
    static Doctor active_doctors[MAX_DOCTORS];
    int count = 0;
    for (int i = 0; i < doctor_count; i++) {
        if (doctors[i].is_active) active_doctors[count++] = doctors[i];
    }
    *doctors_ptr = active_doctors;
    return count;
}

int get_available_doctors(Doctor** doctors_ptr) {
    static Doctor available_doctors[MAX_DOCTORS];
    int count = 0;
    for (int i = 0; i < doctor_count; i++) {
        if (doctors[i].is_active && doctors[i].is_available) available_doctors[count++] = doctors[i];
    }
    *doctors_ptr = available_doctors;
    return count;
}

const char* get_specialization_name(Specialization spec) {
    switch (spec) {
        case SPEC_GENERAL: return "General Medicine";
        case SPEC_CARDIOLOGY: return "Cardiology";
        case SPEC_NEUROLOGY: return "Neurology";
        case SPEC_ORTHOPEDICS: return "Orthopedics";
        case SPEC_PEDIATRICS: return "Pediatrics";
        case SPEC_GYNECOLOGY: return "Gynecology";
        case SPEC_DERMATOLOGY: return "Dermatology";
        case SPEC_ENT: return "ENT";
        case SPEC_OPHTHALMOLOGY: return "Ophthalmology";
        case SPEC_PSYCHIATRY: return "Psychiatry";
        default: return "Unknown";
    }
}

void save_doctors_to_file() {
    FILE* file = fopen(DOCTORS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(DOCTORS_FILE, "wb");
        if (!file) return;
    }
    fwrite(&doctor_count, sizeof(int), 1, file);
    fwrite(doctors, sizeof(Doctor), doctor_count, file);
    fclose(file);
}

void load_doctors_from_file() {
    FILE* file = fopen(DOCTORS_FILE, "rb");
    if (!file) { doctor_count = 0; return; }
    fread(&doctor_count, sizeof(int), 1, file);
    fread(doctors, sizeof(Doctor), doctor_count, file);
    fclose(file);
}
