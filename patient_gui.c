#include "patient.h"
#include "auth.h"
#include "doctor.h"
#include <stdlib.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* name_entry;
    GtkWidget* age_entry;
    GtkWidget* gender_combo;
    GtkWidget* blood_combo;
    GtkWidget* phone_entry;
    GtkWidget* email_entry;
    GtkWidget* address_text;
    GtkWidget* emergency_name_entry;
    GtkWidget* emergency_phone_entry;
    GtkWidget* parent_window;
    int patient_id;
} PatientDialogData;

static void on_assign_doctor_clicked(GtkWidget* widget, gpointer data);

static void on_save_patient_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    PatientDialogData* dialog_data = (PatientDialogData*)data;
    
    Patient patient;
    patient.id = dialog_data->patient_id;
    
    strcpy(patient.name, gtk_entry_get_text(GTK_ENTRY(dialog_data->name_entry)));
    patient.age = atoi(gtk_entry_get_text(GTK_ENTRY(dialog_data->age_entry)));
    patient.gender = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->gender_combo));
    patient.blood_group = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->blood_combo));
    strcpy(patient.phone, gtk_entry_get_text(GTK_ENTRY(dialog_data->phone_entry)));
    strcpy(patient.email, gtk_entry_get_text(GTK_ENTRY(dialog_data->email_entry)));
    strcpy(patient.emergency_contact_name, gtk_entry_get_text(GTK_ENTRY(dialog_data->emergency_name_entry)));
    strcpy(patient.emergency_contact, gtk_entry_get_text(GTK_ENTRY(dialog_data->emergency_phone_entry)));
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->address_text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char* address = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    strcpy(patient.address, address);
    g_free(address);
    
    if (strlen(patient.name) == 0) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter patient name");
        return;
    }
    if (patient.age <= 0 || patient.age > 150) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter valid age (1-150)");
        return;
    }
    if (!is_valid_phone(patient.phone)) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter valid phone number");
        return;
    }
    
    int success = (dialog_data->patient_id == 0) ? add_patient(&patient) : update_patient(&patient);
    
    if (success) {
        // Disconnect the destroy signal to prevent double-free
        g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
        // Free the data ourselves
        g_free(dialog_data);
        // Now destroy the dialog
        gtk_widget_destroy(dialog_data->window);
    } else {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Failed to save patient");
    }
}

static void on_cancel_patient_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    PatientDialogData* dialog_data = (PatientDialogData*)data;
    
    // Disconnect the destroy signal to prevent double-free
    g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
    // Free the data ourselves
    g_free(dialog_data);
    // Now destroy the dialog
    gtk_widget_destroy(dialog_data->window);
}


void show_add_patient_dialog(GtkWidget* parent_window) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Add New Patient");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 650);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(dialog), scrolled);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(scrolled), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Patient Registration</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Full Name:"), 0, row, 1, 1);
    GtkWidget* name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter patient name");
    gtk_widget_set_hexpand(name_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Age:"), 0, row, 1, 1);
    GtkWidget* age_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(age_entry), "Enter age");
    gtk_grid_attach(GTK_GRID(grid), age_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Gender:"), 0, row, 1, 1);
    GtkWidget* gender_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Male");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Female");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gender_combo), "Other");
    gtk_combo_box_set_active(GTK_COMBO_BOX(gender_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), gender_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Blood Group:"), 0, row, 1, 1);
    GtkWidget* blood_combo = gtk_combo_box_text_new();
    const char* blood_groups[] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    for (int i = 0; i < 8; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(blood_combo), blood_groups[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(blood_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), blood_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Phone:"), 0, row, 1, 1);
    GtkWidget* phone_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(phone_entry), "Enter phone number");
    gtk_grid_attach(GTK_GRID(grid), phone_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Email:"), 0, row, 1, 1);
    GtkWidget* email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Enter email (optional)");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Address:"), 0, row, 1, 1);
    GtkWidget* address_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(address_text), GTK_WRAP_WORD);
    GtkWidget* address_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(address_scroll, -1, 80);
    gtk_container_add(GTK_CONTAINER(address_scroll), address_text);
    gtk_grid_attach(GTK_GRID(grid), address_scroll, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Emergency Contact:"), 0, row, 1, 1);
    GtkWidget* emerg_name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emerg_name_entry), "Contact person name");
    gtk_grid_attach(GTK_GRID(grid), emerg_name_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Emergency Phone:"), 0, row, 1, 1);
    GtkWidget* emerg_phone_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(emerg_phone_entry), "Emergency contact number");
    gtk_grid_attach(GTK_GRID(grid), emerg_phone_entry, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* save_button = gtk_button_new_with_label("💾 Save Patient");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    PatientDialogData* dialog_data = g_new(PatientDialogData, 1);
    dialog_data->window = dialog;
    dialog_data->name_entry = name_entry;
    dialog_data->age_entry = age_entry;
    dialog_data->gender_combo = gender_combo;
    dialog_data->blood_combo = blood_combo;
    dialog_data->phone_entry = phone_entry;
    dialog_data->email_entry = email_entry;
    dialog_data->address_text = address_text;
    dialog_data->emergency_name_entry = emerg_name_entry;
    dialog_data->emergency_phone_entry = emerg_phone_entry;
    dialog_data->parent_window = parent_window;
    dialog_data->patient_id = 0;
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_patient_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_patient_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(g_free), dialog_data);
    
    gtk_widget_show_all(dialog);
}

GtkWidget* create_patient_list_view(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>👥 Patient Management</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget* add_button = gtk_button_new_with_label("➕ Add Patient");
    gtk_box_pack_end(GTK_BOX(header_box), add_button, FALSE, FALSE, 0);
    
    GtkWidget* search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "🔍 Search patients by name or phone...");
    gtk_box_pack_start(GTK_BOX(vbox), search_entry, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, 
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Name", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Age", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Gender", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Blood Group", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Phone", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Reg. Date", renderer, "text", 6, NULL);
    
    Patient* all_patients;
    int count = get_all_patients(&all_patients);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, all_patients[i].id,
                          1, all_patients[i].name,
                          2, all_patients[i].age,
                          3, get_gender_name(all_patients[i].gender),
                          4, get_blood_group_name(all_patients[i].blood_group),
                          5, all_patients[i].phone,
                          6, date_to_string(all_patients[i].registration_date),
                          -1);
    }
    
    g_signal_connect_swapped(add_button, "clicked", G_CALLBACK(show_add_patient_dialog), parent_window);
    
    return vbox;
}

void show_assign_doctor_dialog(GtkWidget* parent_window, int patient_id) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Assign Doctor to Patient");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 250);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>👨‍⚕️ Assign Primary Doctor</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    Patient* patient = get_patient_by_id(patient_id);
    if (!patient) {
        gtk_widget_destroy(dialog);
        return;
    }
    
    char patient_info[200];
    snprintf(patient_info, sizeof(patient_info), "Patient: %s (ID: %d)", patient->name, patient->id);
    GtkWidget* patient_label = gtk_label_new(patient_info);
    gtk_box_pack_start(GTK_BOX(vbox), patient_label, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Select Doctor:"), 0, 0, 1, 1);
    GtkWidget* doctor_combo = gtk_combo_box_text_new();
    
    Doctor* doctors;
    int doctor_count = get_all_doctors(&doctors);
    for (int i = 0; i < doctor_count; i++) {
        char label[200];
        snprintf(label, sizeof(label), "Dr. %s - %s", doctors[i].name,
                get_specialization_name(doctors[i].specialization));
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doctor_combo), label);
    }
    
    // Select current doctor if assigned
    if (patient->primary_doctor_id > 0) {
        for (int i = 0; i < doctor_count; i++) {
            if (doctors[i].id == patient->primary_doctor_id) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), i);
                break;
            }
        }
    } else if (doctor_count > 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), 0);
    }
    
    gtk_widget_set_hexpand(doctor_combo, TRUE);
    gtk_grid_attach(GTK_GRID(grid), doctor_combo, 1, 0, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* assign_button = gtk_button_new_with_label("✓ Assign");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), assign_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    // Store data for callback
    static int callback_data[3];
    callback_data[0] = patient_id;
    callback_data[1] = (int)(long)doctor_combo;
    callback_data[2] = (int)(long)dialog;
    
    g_signal_connect(assign_button, "clicked", G_CALLBACK(on_assign_doctor_clicked), callback_data);
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
    
    gtk_widget_show_all(dialog);
}

static void on_assign_doctor_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    int* callback_data = (int*)data;
    int patient_id = callback_data[0];
    GtkWidget* doctor_combo = (GtkWidget*)(long)callback_data[1];
    GtkWidget* dialog = (GtkWidget*)(long)callback_data[2];
    
    int doctor_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(doctor_combo));
    if (doctor_idx < 0) {
        show_error_dialog(GTK_WINDOW(dialog), "Please select a doctor");
        return;
    }
    
    Doctor* doctors;
    get_all_doctors(&doctors);
    int doctor_id = doctors[doctor_idx].id;
    
    if (assign_patient_to_doctor(patient_id, doctor_id)) {
        gtk_widget_destroy(dialog);
    } else {
        show_error_dialog(GTK_WINDOW(dialog), "Failed to assign doctor");
    }
}
