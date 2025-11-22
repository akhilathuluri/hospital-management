#include "doctor_panel.h"
#include "patient.h"
#include "appointment.h"
#include "admission.h"
#include "doctor.h"
#include <string.h>

DoctorStats get_doctor_statistics(int doctor_id) {
    DoctorStats stats = {0};
    
    // Count appointments
    Appointment* appointments;
    int appt_count = get_appointments_by_doctor(doctor_id, &appointments);
    
    Date today;
    get_current_date(&today);
    
    for (int i = 0; i < appt_count; i++) {
        if (compare_dates(appointments[i].appointment_date, today) == 0) {
            stats.appointments_today++;
        }
        if (appointments[i].status == APPT_COMPLETED) {
            stats.completed_appointments++;
        }
    }
    
    // Count unique patients from appointments
    int unique_patients[1000] = {0};
    int patient_count = 0;
    for (int i = 0; i < appt_count; i++) {
        int found = 0;
        for (int j = 0; j < patient_count; j++) {
            if (unique_patients[j] == appointments[i].patient_id) {
                found = 1;
                break;
            }
        }
        if (!found) {
            unique_patients[patient_count++] = appointments[i].patient_id;
        }
    }
    stats.total_my_patients = patient_count;
    
    // Count admitted patients
    Admission* admissions;
    stats.admitted_patients = get_admissions_by_doctor(doctor_id, &admissions);
    
    return stats;
}

GtkWidget* create_doctor_dashboard(GtkWidget* parent_window) {
    (void)parent_window;
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>👨‍⚕️ Doctor Dashboard</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    char welcome_msg[200];
    snprintf(welcome_msg, sizeof(welcome_msg), "Welcome, Dr. %s", g_session.current_user.full_name);
    GtkWidget* welcome_label = gtk_label_new(welcome_msg);
    gtk_widget_set_halign(welcome_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), welcome_label, FALSE, FALSE, 0);
    
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 10);
    
    // Get doctor ID from user session
    int doctor_id = g_session.current_user.doctor_id;
    
    // If no doctor_id linked, try to find by name (fallback)
    if (doctor_id == 0) {
        Doctor* all_doctors;
        int doctor_count = get_all_doctors(&all_doctors);
        for (int i = 0; i < doctor_count; i++) {
            if (strstr(all_doctors[i].name, g_session.current_user.full_name) != NULL) {
                doctor_id = all_doctors[i].id;
                break;
            }
        }
    }
    
    DoctorStats stats = get_doctor_statistics(doctor_id);
    
    char stats_text[500];
    snprintf(stats_text, sizeof(stats_text),
             "📊 Your Statistics:\n\n"
             "My Patients: %d\n"
             "Appointments Today: %d\n"
             "Admitted Patients: %d\n"
             "Completed Appointments: %d",
             stats.total_my_patients, stats.appointments_today,
             stats.admitted_patients, stats.completed_appointments);
    
    GtkWidget* stats_label = gtk_label_new(stats_text);
    gtk_widget_set_halign(stats_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), stats_label, FALSE, FALSE, 10);
    
    GtkWidget* info_label = gtk_label_new("\n💡 Use the sidebar to view:\n"
                                          "• My Patients - Patients you've treated\n"
                                          "• My Appointments - Your scheduled appointments\n"
                                          "• My Admissions - Patients you're treating in hospital");
    gtk_widget_set_halign(info_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 10);
    
    return vbox;
}

GtkWidget* create_doctor_patients_view(GtkWidget* parent_window) {
    (void)parent_window;
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>👥 My Patients</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Patient ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Name", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Age", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Gender", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Phone", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Last Visit", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Assignment", renderer, "text", 6, NULL);
    
    // Find doctor ID
    Doctor* all_doctors;
    int doctor_count = get_all_doctors(&all_doctors);
    int doctor_id = 0;
    
    for (int i = 0; i < doctor_count; i++) {
        if (strstr(all_doctors[i].name, g_session.current_user.full_name) != NULL) {
            doctor_id = all_doctors[i].id;
            break;
        }
    }
    
    // Get assigned patients (primary doctor)
    Patient* assigned_patients;
    int assigned_count = get_patients_by_doctor(doctor_id, &assigned_patients);
    
    for (int i = 0; i < assigned_count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, assigned_patients[i].id,
                          1, assigned_patients[i].name,
                          2, assigned_patients[i].age,
                          3, get_gender_name(assigned_patients[i].gender),
                          4, assigned_patients[i].phone,
                          5, date_to_string(assigned_patients[i].registration_date),
                          6, "Primary Doctor",
                          -1);
    }
    
    // Also get patients from appointments
    Appointment* appointments;
    int appt_count = get_appointments_by_doctor(doctor_id, &appointments);
    
    int added_patients[1000] = {0};
    int added_count = 0;
    
    // Add assigned patients to the "already added" list
    for (int i = 0; i < assigned_count; i++) {
        added_patients[added_count++] = assigned_patients[i].id;
    }
    
    for (int i = 0; i < appt_count; i++) {
        int already_added = 0;
        for (int j = 0; j < added_count; j++) {
            if (added_patients[j] == appointments[i].patient_id) {
                already_added = 1;
                break;
            }
        }
        
        if (!already_added) {
            Patient* patient = get_patient_by_id(appointments[i].patient_id);
            if (patient) {
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                                  0, patient->id,
                                  1, patient->name,
                                  2, patient->age,
                                  3, get_gender_name(patient->gender),
                                  4, patient->phone,
                                  5, date_to_string(appointments[i].appointment_date),
                                  6, "Via Appointment",
                                  -1);
                added_patients[added_count++] = patient->id;
            }
        }
    }
    
    return vbox;
}

GtkWidget* create_doctor_appointments_view(GtkWidget* parent_window) {
    (void)parent_window;
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>📅 My Appointments</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Patient", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Date", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Time", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Reason", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Status", renderer, "text", 5, NULL);
    
    // Find doctor ID
    Doctor* all_doctors;
    int doctor_count = get_all_doctors(&all_doctors);
    int doctor_id = 0;
    
    for (int i = 0; i < doctor_count; i++) {
        if (strstr(all_doctors[i].name, g_session.current_user.full_name) != NULL) {
            doctor_id = all_doctors[i].id;
            break;
        }
    }
    
    Appointment* appointments;
    int count = get_appointments_by_doctor(doctor_id, &appointments);
    
    for (int i = 0; i < count; i++) {
        Patient* patient = get_patient_by_id(appointments[i].patient_id);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, appointments[i].id,
                          1, patient ? patient->name : "Unknown",
                          2, date_to_string(appointments[i].appointment_date),
                          3, time_to_string(appointments[i].appointment_time),
                          4, appointments[i].reason,
                          5, get_appointment_status_name(appointments[i].status),
                          -1);
    }
    
    return vbox;
}

GtkWidget* create_doctor_admissions_view(GtkWidget* parent_window) {
    (void)parent_window;
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>🏥 My Admitted Patients</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Patient", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Ward", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Bed", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Admitted", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Diagnosis", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Days", renderer, "text", 6, NULL);
    
    // Find doctor ID
    Doctor* all_doctors;
    int doctor_count = get_all_doctors(&all_doctors);
    int doctor_id = 0;
    
    for (int i = 0; i < doctor_count; i++) {
        if (strstr(all_doctors[i].name, g_session.current_user.full_name) != NULL) {
            doctor_id = all_doctors[i].id;
            break;
        }
    }
    
    Admission* admissions;
    int count = get_admissions_by_doctor(doctor_id, &admissions);
    
    for (int i = 0; i < count; i++) {
        if (admissions[i].status != ADMISSION_ACTIVE) continue;
        
        Patient* patient = get_patient_by_id(admissions[i].patient_id);
        Bed* bed = get_bed_by_id(admissions[i].bed_id);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, admissions[i].id,
                          1, patient ? patient->name : "Unknown",
                          2, bed ? get_ward_name(bed->ward) : "Unknown",
                          3, bed ? bed->bed_number : 0,
                          4, date_to_string(admissions[i].admission_date),
                          5, admissions[i].diagnosis,
                          6, calculate_length_of_stay(&admissions[i]),
                          -1);
    }
    
    return vbox;
}
