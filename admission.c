#include "admission.h"
#include "appointment.h"
#include <string.h>

#define MAX_BEDS 50
#define MAX_ADMISSIONS 1000
#define BEDS_FILE "data/beds.dat"
#define ADMISSIONS_FILE "data/admissions.dat"

static Bed beds[MAX_BEDS];
static int bed_count = 0;
static Admission admissions[MAX_ADMISSIONS];
static int admission_count = 0;

void init_beds() {
    bed_count = 0;
    
    // Create 10 beds for each ward type
    for (int ward = WARD_GENERAL; ward <= WARD_MATERNITY; ward++) {
        for (int i = 1; i <= 10; i++) {
            beds[bed_count].id = bed_count + 1;
            beds[bed_count].bed_number = i;
            beds[bed_count].ward = ward;
            beds[bed_count].status = BED_AVAILABLE;
            beds[bed_count].current_patient_id = 0;
            bed_count++;
        }
    }
    save_beds_to_file();
}

void init_admission_system() {
    load_beds_from_file();
    if (bed_count == 0) {
        init_beds();
    }
    load_admissions_from_file();
}

int get_available_beds_by_ward(WardType ward, Bed** beds_ptr) {
    static Bed available_beds[MAX_BEDS];
    int count = 0;
    for (int i = 0; i < bed_count; i++) {
        if (beds[i].ward == ward && beds[i].status == BED_AVAILABLE) {
            available_beds[count++] = beds[i];
        }
    }
    *beds_ptr = available_beds;
    return count;
}

int get_all_beds(Bed** beds_ptr) {
    *beds_ptr = beds;
    return bed_count;
}

Bed* get_bed_by_id(int bed_id) {
    for (int i = 0; i < bed_count; i++) {
        if (beds[i].id == bed_id) return &beds[i];
    }
    return NULL;
}

int update_bed_status(int bed_id, BedStatus status) {
    for (int i = 0; i < bed_count; i++) {
        if (beds[i].id == bed_id) {
            beds[i].status = status;
            if (status == BED_AVAILABLE) {
                beds[i].current_patient_id = 0;
            }
            save_beds_to_file();
            return 1;
        }
    }
    return 0;
}

int admit_patient(Admission* admission) {
    if (admission_count >= MAX_ADMISSIONS) return 0;
    
    // Check if bed is available
    Bed* bed = get_bed_by_id(admission->bed_id);
    if (!bed || bed->status != BED_AVAILABLE) return 0;
    
    admission->id = admission_count + 1;
    get_current_date(&admission->admission_date);
    get_current_time(&admission->admission_time);
    admission->status = ADMISSION_ACTIVE;
    admission->is_active = 1;
    strcpy(admission->discharge_summary, "");
    admission->actual_discharge_date.day = 0;
    admission->total_charges = 0.0;
    
    admissions[admission_count++] = *admission;
    
    // Mark bed as occupied
    bed->status = BED_OCCUPIED;
    bed->current_patient_id = admission->patient_id;
    save_beds_to_file();
    save_admissions_to_file();
    
    return 1;
}

int discharge_patient(int admission_id, const char* discharge_summary, double charges) {
    for (int i = 0; i < admission_count; i++) {
        if (admissions[i].id == admission_id && admissions[i].status == ADMISSION_ACTIVE) {
            get_current_date(&admissions[i].actual_discharge_date);
            get_current_time(&admissions[i].discharge_time);
            strcpy(admissions[i].discharge_summary, discharge_summary);
            admissions[i].total_charges = charges;
            admissions[i].status = ADMISSION_DISCHARGED;
            admissions[i].is_active = 0;
            
            // Free the bed
            update_bed_status(admissions[i].bed_id, BED_AVAILABLE);
            
            // Mark related appointments as completed
            Appointment* patient_appointments;
            int appt_count = get_appointments_by_patient(admissions[i].patient_id, &patient_appointments);
            for (int j = 0; j < appt_count; j++) {
                if (patient_appointments[j].doctor_id == admissions[i].doctor_id &&
                    (patient_appointments[j].status == APPT_PENDING || 
                     patient_appointments[j].status == APPT_CONFIRMED)) {
                    patient_appointments[j].status = APPT_COMPLETED;
                    update_appointment(&patient_appointments[j]);
                }
            }
            
            save_admissions_to_file();
            return 1;
        }
    }
    return 0;
}

int transfer_patient(int admission_id, int new_bed_id) {
    Admission* admission = get_admission_by_id(admission_id);
    if (!admission || admission->status != ADMISSION_ACTIVE) return 0;
    
    Bed* new_bed = get_bed_by_id(new_bed_id);
    if (!new_bed || new_bed->status != BED_AVAILABLE) return 0;
    
    // Free old bed
    update_bed_status(admission->bed_id, BED_AVAILABLE);
    
    // Assign new bed
    admission->bed_id = new_bed_id;
    new_bed->status = BED_OCCUPIED;
    new_bed->current_patient_id = admission->patient_id;
    admission->status = ADMISSION_TRANSFERRED;
    
    save_beds_to_file();
    save_admissions_to_file();
    return 1;
}

Admission* get_admission_by_id(int admission_id) {
    for (int i = 0; i < admission_count; i++) {
        if (admissions[i].id == admission_id) return &admissions[i];
    }
    return NULL;
}

int get_active_admissions(Admission** admissions_ptr) {
    static Admission active_admissions[MAX_ADMISSIONS];
    int count = 0;
    for (int i = 0; i < admission_count; i++) {
        if (admissions[i].status == ADMISSION_ACTIVE) {
            active_admissions[count++] = admissions[i];
        }
    }
    *admissions_ptr = active_admissions;
    return count;
}

int get_admissions_by_patient(int patient_id, Admission** admissions_ptr) {
    static Admission patient_admissions[MAX_ADMISSIONS];
    int count = 0;
    for (int i = 0; i < admission_count; i++) {
        if (admissions[i].patient_id == patient_id) {
            patient_admissions[count++] = admissions[i];
        }
    }
    *admissions_ptr = patient_admissions;
    return count;
}

int get_admissions_by_doctor(int doctor_id, Admission** admissions_ptr) {
    static Admission doctor_admissions[MAX_ADMISSIONS];
    int count = 0;
    for (int i = 0; i < admission_count; i++) {
        if (admissions[i].doctor_id == doctor_id) {
            doctor_admissions[count++] = admissions[i];
        }
    }
    *admissions_ptr = doctor_admissions;
    return count;
}

int calculate_length_of_stay(Admission* admission) {
    if (!admission) return 0;
    
    Date end_date;
    
    // Check if discharged and has valid discharge date
    if (admission->status == ADMISSION_DISCHARGED && 
        admission->actual_discharge_date.year > 0) {
        end_date = admission->actual_discharge_date;
    } 
    // Otherwise use expected discharge if valid
    else if (admission->expected_discharge_date.year > 0) {
        end_date = admission->expected_discharge_date;
    } 
    // Fall back to current date
    else {
        get_current_date(&end_date);
    }
    
    // Validate dates
    if (admission->admission_date.year == 0 || end_date.year == 0) {
        return 0;
    }
    
    // Validate date components
    if (admission->admission_date.month < 1 || admission->admission_date.month > 12 ||
        admission->admission_date.day < 1 || admission->admission_date.day > 31 ||
        end_date.month < 1 || end_date.month > 12 ||
        end_date.day < 1 || end_date.day > 31) {
        return 0;
    }
    
    // Simple day calculation (can be improved)
    int days = (end_date.year - admission->admission_date.year) * 365 +
               (end_date.month - admission->admission_date.month) * 30 +
               (end_date.day - admission->admission_date.day);
    
    return days > 0 ? days : 1;
}

const char* get_ward_name(WardType ward) {
    switch (ward) {
        case WARD_GENERAL: return "General Ward";
        case WARD_ICU: return "ICU";
        case WARD_EMERGENCY: return "Emergency";
        case WARD_PEDIATRIC: return "Pediatric";
        case WARD_MATERNITY: return "Maternity";
        default: return "Unknown";
    }
}

const char* get_bed_status_name(BedStatus status) {
    switch (status) {
        case BED_AVAILABLE: return "Available";
        case BED_OCCUPIED: return "Occupied";
        case BED_MAINTENANCE: return "Maintenance";
        default: return "Unknown";
    }
}

const char* get_admission_status_name(AdmissionStatus status) {
    switch (status) {
        case ADMISSION_ACTIVE: return "Active";
        case ADMISSION_DISCHARGED: return "Discharged";
        case ADMISSION_TRANSFERRED: return "Transferred";
        default: return "Unknown";
    }
}

void save_admissions_to_file() {
    FILE* file = fopen(ADMISSIONS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(ADMISSIONS_FILE, "wb");
        if (!file) return;
    }
    fwrite(&admission_count, sizeof(int), 1, file);
    fwrite(admissions, sizeof(Admission), admission_count, file);
    fclose(file);
}

void load_admissions_from_file() {
    FILE* file = fopen(ADMISSIONS_FILE, "rb");
    if (!file) { admission_count = 0; return; }
    fread(&admission_count, sizeof(int), 1, file);
    fread(admissions, sizeof(Admission), admission_count, file);
    fclose(file);
}

void save_beds_to_file() {
    FILE* file = fopen(BEDS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(BEDS_FILE, "wb");
        if (!file) return;
    }
    fwrite(&bed_count, sizeof(int), 1, file);
    fwrite(beds, sizeof(Bed), bed_count, file);
    fclose(file);
}

void load_beds_from_file() {
    FILE* file = fopen(BEDS_FILE, "rb");
    if (!file) { bed_count = 0; return; }
    fread(&bed_count, sizeof(int), 1, file);
    fread(beds, sizeof(Bed), bed_count, file);
    fclose(file);
}
