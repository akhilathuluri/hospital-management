#include "doctor.h"
#include "auth.h"
#include <stdlib.h>

typedef struct {
    GtkWidget* window;
    GtkWidget* name_entry;
    GtkWidget* spec_combo;
    GtkWidget* qual_entry;
    GtkWidget* exp_entry;
    GtkWidget* phone_entry;
    GtkWidget* email_entry;
    GtkWidget* fee_entry;
    int doctor_id;
} DoctorDialogData;

static void on_save_doctor_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    DoctorDialogData* dialog_data = (DoctorDialogData*)data;
    
    Doctor doctor;
    doctor.id = dialog_data->doctor_id;
    
    strcpy(doctor.name, gtk_entry_get_text(GTK_ENTRY(dialog_data->name_entry)));
    doctor.specialization = gtk_combo_box_get_active(GTK_COMBO_BOX(dialog_data->spec_combo));
    strcpy(doctor.qualifications, gtk_entry_get_text(GTK_ENTRY(dialog_data->qual_entry)));
    doctor.experience_years = atoi(gtk_entry_get_text(GTK_ENTRY(dialog_data->exp_entry)));
    strcpy(doctor.phone, gtk_entry_get_text(GTK_ENTRY(dialog_data->phone_entry)));
    strcpy(doctor.email, gtk_entry_get_text(GTK_ENTRY(dialog_data->email_entry)));
    doctor.consultation_fee = atof(gtk_entry_get_text(GTK_ENTRY(dialog_data->fee_entry)));
    
    if (strlen(doctor.name) == 0) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter doctor name");
        return;
    }
    if (!is_valid_phone(doctor.phone)) {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Please enter valid phone number");
        return;
    }
    
    int success = (dialog_data->doctor_id == 0) ? add_doctor(&doctor) : update_doctor(&doctor);
    
    if (success) {
        // Disconnect the destroy signal to prevent double-free
        g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
        // Free the data ourselves
        g_free(dialog_data);
        // Now destroy the dialog
        gtk_widget_destroy(dialog_data->window);
    } else {
        show_error_dialog(GTK_WINDOW(dialog_data->window), "Failed to save doctor");
    }
}

static void on_cancel_doctor_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    DoctorDialogData* dialog_data = (DoctorDialogData*)data;
    
    g_signal_handlers_disconnect_by_func(dialog_data->window, G_CALLBACK(g_free), dialog_data);
    g_free(dialog_data);
    gtk_widget_destroy(dialog_data->window);
}


void show_add_doctor_dialog(GtkWidget* parent_window) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Add New Doctor");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 550);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Doctor Registration</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Name:"), 0, row, 1, 1);
    GtkWidget* name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Enter doctor name");
    gtk_widget_set_hexpand(name_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Specialization:"), 0, row, 1, 1);
    GtkWidget* spec_combo = gtk_combo_box_text_new();
    const char* specs[] = {"General Medicine", "Cardiology", "Neurology", "Orthopedics", 
                          "Pediatrics", "Gynecology", "Dermatology", "ENT", "Ophthalmology", "Psychiatry"};
    for (int i = 0; i < 10; i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(spec_combo), specs[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(spec_combo), 0);
    gtk_grid_attach(GTK_GRID(grid), spec_combo, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Qualifications:"), 0, row, 1, 1);
    GtkWidget* qual_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(qual_entry), "e.g., MBBS, MD");
    gtk_grid_attach(GTK_GRID(grid), qual_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Experience (years):"), 0, row, 1, 1);
    GtkWidget* exp_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(exp_entry), "Years of experience");
    gtk_grid_attach(GTK_GRID(grid), exp_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Phone:"), 0, row, 1, 1);
    GtkWidget* phone_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(phone_entry), "Contact number");
    gtk_grid_attach(GTK_GRID(grid), phone_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Email:"), 0, row, 1, 1);
    GtkWidget* email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Email address");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Consultation Fee:"), 0, row, 1, 1);
    GtkWidget* fee_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(fee_entry), "Fee amount");
    gtk_grid_attach(GTK_GRID(grid), fee_entry, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* save_button = gtk_button_new_with_label("💾 Save Doctor");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    DoctorDialogData* dialog_data = g_new(DoctorDialogData, 1);
    dialog_data->window = dialog;
    dialog_data->name_entry = name_entry;
    dialog_data->spec_combo = spec_combo;
    dialog_data->qual_entry = qual_entry;
    dialog_data->exp_entry = exp_entry;
    dialog_data->phone_entry = phone_entry;
    dialog_data->email_entry = email_entry;
    dialog_data->fee_entry = fee_entry;
    dialog_data->doctor_id = 0;
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_doctor_clicked), dialog_data);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_doctor_clicked), dialog_data);
    g_signal_connect(dialog, "destroy", G_CALLBACK(g_free), dialog_data);
    
    gtk_widget_show_all(dialog);
}

GtkWidget* create_doctor_list_view(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>👨‍⚕️ Doctor Management</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget* add_button = gtk_button_new_with_label("➕ Add Doctor");
    gtk_box_pack_end(GTK_BOX(header_box), add_button, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_DOUBLE);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Name", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Specialization", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Qualifications", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Experience", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Phone", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Fee", renderer, "text", 6, NULL);
    
    Doctor* all_doctors;
    int count = get_all_doctors(&all_doctors);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, all_doctors[i].id,
                          1, all_doctors[i].name,
                          2, get_specialization_name(all_doctors[i].specialization),
                          3, all_doctors[i].qualifications,
                          4, all_doctors[i].experience_years,
                          5, all_doctors[i].phone,
                          6, all_doctors[i].consultation_fee,
                          -1);
    }
    
    g_signal_connect_swapped(add_button, "clicked", G_CALLBACK(show_add_doctor_dialog), parent_window);
    
    return vbox;
}
