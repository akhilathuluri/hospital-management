#include "admission.h"
#include "patient.h"
#include "doctor.h"
#include <stdlib.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* patient_combo;
    GtkWidget* doctor_combo;
    GtkWidget* ward_combo;
    GtkWidget* bed_combo;
    GtkWidget* expected_discharge_entry;
    GtkWidget* reason_text;
    GtkWidget* diagnosis_text;
    Patient* patients;
    int patient_count;
    Doctor* doctors;
    int doctor_count;
} AdmitDialogData;

typedef struct {
    GtkWidget* window;
    GtkWidget* summary_text;
    GtkWidget* charges_entry;
    int admission_id;
} DischargeDialogData;

static void on_ward_changed(GtkWidget* widget, gpointer data);
static void on_admit_patient_clicked(GtkWidget* widget, gpointer data);
static void on_discharge_clicked(GtkWidget* widget, gpointer data);
static void on_cancel_admit_clicked(GtkWidget* widget, gpointer data);
static void on_cancel_discharge_clicked(GtkWidget* widget, gpointer data);
static void on_admission_row_activated(GtkTreeView* tree_view, GtkTreePath* path, 
                                       GtkTreeViewColumn* column, gpointer user_data);

void show_admit_patient_dialog(GtkWidget* parent_window) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Admit Patient");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 550, 550);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>🏥 Admit Patient</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Patient:"), 0, row, 1, 1);
    GtkWidget* patient_combo = gtk_combo_box_text_new();
    Patient* patients;
    int patient_count = get_all_patients(&patients);
    for (int i = 0; i < patient_count; i++) {
        char label[200];
        snprintf(label, sizeof(label), "%s (ID: %d)", patients[i].name, patients[i].id);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(patient_combo), label);
    }
    if (patient_count > 0) gtk_combo_box_set_active(GTK_COMBO_BOX(patient_combo), 0);
    gtk_widget_set_hexpand(patient_combo, TRUE);
    gtk_grid_attach(GTK_GRID(grid), patient_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Doctor:"), 0, row, 1, 1);
    GtkWidget* doctor_combo = gtk_combo_box_text_new();
    Doctor* doctors;
    int doctor_count = get_all_doctors(&doctors);
    for (int i = 0; i < doctor_count; i++) {
        char label[200];
        snprintf(label, sizeof(label), "Dr. %s - %s", doctors[i].name, 
                get_specialization_name(doctors[i].specialization));
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doctor_combo), label);
    }
    if (doctor_count > 0) gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), doctor_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Ward:"), 0, row, 1, 1);
    GtkWidget* ward_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ward_combo), "General Ward");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ward_combo), "ICU");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ward_combo), "Emergency");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ward_combo), "Pediatric");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ward_combo), "Maternity");
    gtk_combo_box_set_active(GTK_COMBO_BOX(ward_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), ward_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Bed:"), 0, row, 1, 1);
    GtkWidget* bed_combo = gtk_combo_box_text_new();
    gtk_grid_attach(GTK_GRID(grid), bed_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Expected Discharge (DD/MM/YYYY):"), 0, row, 1, 1);
    GtkWidget* expected_discharge_entry = gtk_entry_new();
    Date future_date;
    get_current_date(&future_date);
    future_date.day += 3; // Default 3 days
    gtk_entry_set_text(GTK_ENTRY(expected_discharge_entry), date_to_string(future_date));
    gtk_grid_attach(GTK_GRID(grid), expected_discharge_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Admission Reason:"), 0, row, 1, 1);
    GtkWidget* reason_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(reason_text), GTK_WRAP_WORD);
    GtkWidget* reason_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(reason_scroll, -1, 80);
    gtk_container_add(GTK_CONTAINER(reason_scroll), reason_text);
    gtk_grid_attach(GTK_GRID(grid), reason_scroll, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Initial Diagnosis:"), 0, row, 1, 1);
    GtkWidget* diagnosis_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(diagnosis_text), GTK_WRAP_WORD);
    GtkWidget* diagnosis_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(diagnosis_scroll, -1, 80);
    gtk_container_add(GTK_CONTAINER(diagnosis_scroll), diagnosis_text);
    gtk_grid_attach(GTK_GRID(grid), diagnosis_scroll, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* admit_button = gtk_button_new_with_label("🏥 Admit Patient");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), admit_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    AdmitDialogData* dialog_data = g_new(AdmitDialogData, 1);
    dialog_data->window = dialog;
    dialog_data->patient_combo = patient_combo;
    dialog_data->doctor_combo = doctor_combo;
    dialog_data->ward_combo = ward_combo;
    dialog_data->bed_combo = bed_combo;
    dialog_data->expected_discharge_entry = expected_discharge_entry;
    dialog_data->reason_text = reason_text;
    dialog_data->diagnosis_text = diagnosis_text;
    dialog_data->patients = patients;
    dialog_data->patient_count = patient_count;
    dialog_data->doctors = doctors;
    dialog_data->doctor_count = doctor_count;
    
    g_signal_connect(ward_combo, "changed", G_CALLBACK(on_ward_changed), dialog_data);
    g_signal_connect(admit_button, "clicked", G_CALLBACK(on_admit_patient_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_admit_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(g_free), dialog_data);
    
    // Trigger initial bed loading
    on_ward_changed(ward_combo, dialog_data);
    
    gtk_widget_show_all(dialog);
}

static void on_ward_changed(GtkWidget* widget, gpointer data) {
    (void)widget;
    AdmitDialogData* dialog_data = (AdmitDialogData*)data;
    
    int ward_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->ward_combo));
    if (ward_idx < 0) return;
    
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(dialog_data->bed_combo));
    
    Bed* available_beds;
    int count = get_available_beds_by_ward(ward_idx, &available_beds);
    
    for (int i = 0; i < count; i++) {
        char label[100];
        snprintf(label, sizeof(label), "Bed %d (ID: %d)", 
                available_beds[i].bed_number, available_beds[i].id);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(dialog_data->bed_combo), label);
    }
    
    if (count > 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(dialog_data->bed_combo), 0);
    }
}

static void on_admit_patient_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    AdmitDialogData* dialog_data = (AdmitDialogData*)data;
    
    int patient_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->patient_combo));
    int doctor_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->doctor_combo));
    int ward_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->ward_combo));
    int bed_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->bed_combo));
    
    if (patient_idx < 0 || doctor_idx < 0 || bed_idx < 0) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please select patient, doctor, and bed");
        return;
    }
    
    Bed* available_beds;
    get_available_beds_by_ward(ward_idx, &available_beds);
    
    Admission admission;
    admission.patient_id = dialog_data->patients[patient_idx].id;
    admission.doctor_id = dialog_data->doctors[doctor_idx].id;
    admission.bed_id = available_beds[bed_idx].id;
    
    const char* expected_date_str = gtk_entry_get_text(GTK_ENTRY(dialog_data->expected_discharge_entry));
    admission.expected_discharge_date = string_to_date(expected_date_str);
    
    GtkTextBuffer* reason_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->reason_text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(reason_buffer, &start, &end);
    char* reason = gtk_text_buffer_get_text(reason_buffer, &start, &end, FALSE);
    strcpy(admission.admission_reason, reason);
    g_free(reason);
    
    GtkTextBuffer* diagnosis_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->diagnosis_text));
    gtk_text_buffer_get_bounds(diagnosis_buffer, &start, &end);
    char* diagnosis = gtk_text_buffer_get_text(diagnosis_buffer, &start, &end, FALSE);
    strcpy(admission.diagnosis, diagnosis);
    g_free(diagnosis);
    
    if (admit_patient(&admission)) {
        g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
        g_free(dialog_data);
        gtk_widget_destroy(dialog_data->window);
    } else {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Failed to admit patient. Bed may be occupied.");
    }
}

static void on_cancel_admit_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    AdmitDialogData* dialog_data = (AdmitDialogData*)data;
    g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
    g_free(dialog_data);
    gtk_widget_destroy(dialog_data->window);
}

void show_discharge_dialog(GtkWidget* parent_window, int admission_id) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Discharge Patient");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 400);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>📋 Discharge Patient</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Discharge Summary:"), 0, row, 1, 1);
    GtkWidget* summary_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(summary_text), GTK_WRAP_WORD);
    GtkWidget* summary_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(summary_scroll, -1, 150);
    gtk_container_add(GTK_CONTAINER(summary_scroll), summary_text);
    gtk_widget_set_hexpand(summary_scroll, TRUE);
    gtk_grid_attach(GTK_GRID(grid), summary_scroll, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Total Charges:"), 0, row, 1, 1);
    GtkWidget* charges_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(charges_entry), "Enter total charges");
    gtk_entry_set_text(GTK_ENTRY(charges_entry), "0.00");
    gtk_grid_attach(GTK_GRID(grid), charges_entry, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* discharge_button = gtk_button_new_with_label("📋 Discharge");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), discharge_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    DischargeDialogData* dialog_data = g_new(DischargeDialogData, 1);
    dialog_data->window = dialog;
    dialog_data->summary_text = summary_text;
    dialog_data->charges_entry = charges_entry;
    dialog_data->admission_id = admission_id;
    
    g_signal_connect(discharge_button, "clicked", G_CALLBACK(on_discharge_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_discharge_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(g_free), dialog_data);
    
    gtk_widget_show_all(dialog);
}

static void on_discharge_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    DischargeDialogData* dialog_data = (DischargeDialogData*)data;
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->summary_text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char* summary = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    double charges = atof(gtk_entry_get_text(GTK_ENTRY(dialog_data->charges_entry)));
    
    if (discharge_patient(dialog_data->admission_id, summary, charges)) {
        g_free(summary);
        g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
        g_free(dialog_data);
        gtk_widget_destroy(dialog_data->window);
    } else {
        g_free(summary);
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Failed to discharge patient");
    }
}

static void on_cancel_discharge_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    DischargeDialogData* dialog_data = (DischargeDialogData*)data;
    g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
    g_free(dialog_data);
    gtk_widget_destroy(dialog_data->window);
}

GtkWidget* create_admission_view(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>🏥 Patient Admissions</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget* info_label = gtk_label_new("💡 Double-click a patient to discharge");
    gtk_box_pack_start(GTK_BOX(header_box), info_label, FALSE, FALSE, 10);
    
    GtkWidget* admit_button = gtk_button_new_with_label("➕ Admit Patient");
    gtk_box_pack_end(GTK_BOX(header_box), admit_button, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(9, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Patient", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Doctor", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Ward", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Bed", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Admitted", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Expected Discharge", renderer, "text", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Status", renderer, "text", 7, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Days", renderer, "text", 8, NULL);
    
    Admission* active_admissions = NULL;
    int count = get_active_admissions(&active_admissions);
    
    // Only process if we have valid data
    if (count > 0 && active_admissions != NULL) {
        for (int i = 0; i < count; i++) {
            Patient* patient = get_patient_by_id(active_admissions[i].patient_id);
            Doctor* doctor = get_doctor_by_id(active_admissions[i].doctor_id);
            Bed* bed = get_bed_by_id(active_admissions[i].bed_id);
            
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                              0, active_admissions[i].id,
                              1, patient ? patient->name : "Unknown",
                              2, doctor ? doctor->name : "Unknown",
                              3, bed ? get_ward_name(bed->ward) : "N/A",
                              4, bed ? bed->bed_number : 0,
                              5, "22/11/2025",  // Hardcoded for now
                              6, "25/11/2025",  // Hardcoded for now
                              7, "Active",      // Hardcoded for now
                              8, 3,             // Hardcoded for now
                              -1);
        }
    }
    
    // Add double-click handler for discharge
    g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_admission_row_activated), parent_window);
    
    g_signal_connect_swapped(admit_button, "clicked", G_CALLBACK(show_admit_patient_dialog), parent_window);
    
    return vbox;
}

static void on_admission_row_activated(GtkTreeView* tree_view, GtkTreePath* path, 
                                       GtkTreeViewColumn* column, gpointer user_data) {
    (void)column;
    GtkWidget* parent_window = (GtkWidget*)user_data;
    GtkTreeModel* model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        int admission_id;
        gtk_tree_model_get(model, &iter, 0, &admission_id, -1);
        show_discharge_dialog(parent_window, admission_id);
    }
}
