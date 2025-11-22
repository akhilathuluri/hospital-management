#include "appointment.h"
#include "patient.h"
#include "doctor.h"
#include <stdlib.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* patient_combo;
    GtkWidget* doctor_combo;
    GtkWidget* date_entry;
    GtkWidget* time_entry;
    GtkWidget* reason_text;
    Patient* patients;
    int patient_count;
    Doctor* doctors;
    int doctor_count;
} AppointmentDialogData;

static void on_book_appointment_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    AppointmentDialogData* dialog_data = (AppointmentDialogData*)data;
    
    int patient_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->patient_combo));
    int doctor_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->doctor_combo));
    
    if (patient_idx < 0 || doctor_idx < 0) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please select both patient and doctor");
        return;
    }
    
    Appointment appointment;
    appointment.patient_id = dialog_data->patients[patient_idx].id;
    appointment.doctor_id = dialog_data->doctors[doctor_idx].id;
    
    const char* date_str = gtk_entry_get_text(GTK_ENTRY(dialog_data->date_entry));
    const char* time_str = gtk_entry_get_text(GTK_ENTRY(dialog_data->time_entry));
    
    appointment.appointment_date = string_to_date(date_str);
    appointment.appointment_time = string_to_time(time_str);
    
    if (!is_valid_date(appointment.appointment_date)) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter valid date (DD/MM/YYYY)");
        return;
    }
    
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog_data->reason_text));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char* reason = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    strcpy(appointment.reason, reason);
    g_free(reason);
    
    strcpy(appointment.notes, "");
    
    if (book_appointment(&appointment)) {
        // Disconnect the destroy signal to prevent double-free
        g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
        // Free the data ourselves
        g_free(dialog_data);
        // Now destroy the dialog
        gtk_widget_destroy(dialog_data->window);
    } else {
        show_error_dialog(GTK_WINDOW(dialog_data->window), 
                         "Failed to book appointment. Time slot may be already taken.");
    }
}

static void on_cancel_appointment_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    AppointmentDialogData* dialog_data = (AppointmentDialogData*)data;
    
    g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
    g_free(dialog_data);
    gtk_widget_destroy(dialog_data->window);
}


void show_book_appointment_dialog(GtkWidget* parent_window) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Book Appointment");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 450);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Book New Appointment</span>");
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
        snprintf(label, sizeof(label), "%s (ID: %d, Phone: %s)", 
                patients[i].name, patients[i].id, patients[i].phone);
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
        snprintf(label, sizeof(label), "Dr. %s - %s (Fee: %.2f)", 
                doctors[i].name, get_specialization_name(doctors[i].specialization), 
                doctors[i].consultation_fee);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(doctor_combo), label);
    }
    if (doctor_count > 0) gtk_combo_box_set_active(GTK_COMBO_BOX(doctor_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), doctor_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Date (DD/MM/YYYY):"), 0, row, 1, 1);
    GtkWidget* date_entry = gtk_entry_new();
    Date today;
    get_current_date(&today);
    gtk_entry_set_text(GTK_ENTRY(date_entry), date_to_string(today));
    gtk_grid_attach(GTK_GRID(grid), date_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Time (HH:MM):"), 0, row, 1, 1);
    GtkWidget* time_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(time_entry), "e.g., 10:30");
    gtk_entry_set_text(GTK_ENTRY(time_entry), "10:00");
    gtk_grid_attach(GTK_GRID(grid), time_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Reason for Visit:"), 0, row, 1, 1);
    GtkWidget* reason_text = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(reason_text), GTK_WRAP_WORD);
    GtkWidget* reason_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(reason_scroll, -1, 100);
    gtk_container_add(GTK_CONTAINER(reason_scroll), reason_text);
    gtk_grid_attach(GTK_GRID(grid), reason_scroll, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* book_button = gtk_button_new_with_label("📅 Book Appointment");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), book_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    AppointmentDialogData* dialog_data = g_new(AppointmentDialogData, 1);
    dialog_data->window = dialog;
    dialog_data->patient_combo = patient_combo;
    dialog_data->doctor_combo = doctor_combo;
    dialog_data->date_entry = date_entry;
    dialog_data->time_entry = time_entry;
    dialog_data->reason_text = reason_text;
    dialog_data->patients = patients;
    dialog_data->patient_count = patient_count;
    dialog_data->doctors = doctors;
    dialog_data->doctor_count = doctor_count;
    
    g_signal_connect(book_button, "clicked", G_CALLBACK(on_book_appointment_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_appointment_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(g_free), dialog_data);
    
    gtk_widget_show_all(dialog);
}

GtkWidget* create_appointment_view(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>📅 Appointment Management</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget* book_button = gtk_button_new_with_label("➕ Book Appointment");
    gtk_box_pack_end(GTK_BOX(header_box), book_button, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Patient", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Doctor", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Date", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Time", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Status", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Reason", renderer, "text", 6, NULL);
    
    // Get all appointments
    Appointment* appointments;
    int count = get_all_appointments(&appointments);
    
    for (int i = 0; i < count; i++) {
        Patient* patient = get_patient_by_id(appointments[i].patient_id);
        Doctor* doctor = get_doctor_by_id(appointments[i].doctor_id);
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, appointments[i].id,
                          1, patient ? patient->name : "Unknown",
                          2, doctor ? doctor->name : "Unknown",
                          3, date_to_string(appointments[i].appointment_date),
                          4, time_to_string(appointments[i].appointment_time),
                          5, get_appointment_status_name(appointments[i].status),
                          6, appointments[i].reason,
                          -1);
    }
    
    g_signal_connect_swapped(book_button, "clicked", G_CALLBACK(show_book_appointment_dialog), parent_window);
    
    return vbox;
}
