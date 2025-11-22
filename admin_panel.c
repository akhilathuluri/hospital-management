#include "admin_panel.h"
#include "patient.h"
#include "doctor.h"
#include "appointment.h"

static void on_save_user_clicked(GtkWidget* widget, gpointer data);

void init_admin_panel() {}

DashboardStats get_dashboard_statistics() {
    DashboardStats stats = {0};
    Patient* patients;
    stats.total_patients = get_all_patients(&patients);
    Doctor* doctors;
    stats.total_doctors = get_all_doctors(&doctors);
    Date today;
    get_current_date(&today);
    Appointment* today_appointments;
    stats.total_appointments_today = get_appointments_by_date(today, &today_appointments);
    for (int i = 0; i < stats.total_appointments_today; i++) {
        if (today_appointments[i].status == APPT_PENDING) stats.pending_appointments++;
    }
    User* users;
    stats.active_users = get_all_users(&users);
    stats.total_revenue = 0.0;
    return stats;
}

GtkWidget* create_admin_dashboard(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>Admin Dashboard</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    
    char welcome_msg[200];
    snprintf(welcome_msg, sizeof(welcome_msg), "Welcome, %s (%s)", 
             g_session.current_user.full_name, get_role_name(g_session.current_user.role));
    GtkWidget* welcome_label = gtk_label_new(welcome_msg);
    gtk_widget_set_halign(welcome_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), welcome_label, FALSE, FALSE, 0);
    
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 10);
    
    DashboardStats stats = get_dashboard_statistics();
    
    char stats_text[500];
    snprintf(stats_text, sizeof(stats_text),
             "📊 Statistics:\n\n"
             "Total Patients: %d\n"
             "Total Doctors: %d\n"
             "Appointments Today: %d\n"
             "Pending Appointments: %d\n"
             "Active Users: %d",
             stats.total_patients, stats.total_doctors,
             stats.total_appointments_today, stats.pending_appointments, stats.active_users);
    
    GtkWidget* stats_label = gtk_label_new(stats_text);
    gtk_widget_set_halign(stats_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), stats_label, FALSE, FALSE, 10);
    
    return vbox;
}

GtkWidget* create_user_management_view(GtkWidget* parent_window) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), header_box, FALSE, FALSE, 0);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>👤 User Management</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title, FALSE, FALSE, 0);
    
    GtkWidget* add_button = gtk_button_new_with_label("➕ Add User");
    gtk_box_pack_end(GTK_BOX(header_box), add_button, FALSE, FALSE, 0);
    
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    GtkListStore* store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget* tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Username", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Full Name", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Email", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Role", renderer, "text", 4, NULL);
    
    User* all_users;
    int count = get_all_users(&all_users);
    
    for (int i = 0; i < count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, all_users[i].id,
                          1, all_users[i].username,
                          2, all_users[i].full_name,
                          3, all_users[i].email,
                          4, get_role_name(all_users[i].role),
                          -1);
    }
    
    g_signal_connect_swapped(add_button, "clicked", G_CALLBACK(show_add_user_dialog), parent_window);
    
    return vbox;
}

void show_add_user_dialog(GtkWidget* parent_window) {
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Add New User");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 400);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent_window));
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(dialog), vbox);
    
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Create New User</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);
    
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Username:"), 0, row, 1, 1);
    GtkWidget* username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
    gtk_widget_set_hexpand(username_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), username_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Password:"), 0, row, 1, 1);
    GtkWidget* password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_grid_attach(GTK_GRID(grid), password_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Full Name:"), 0, row, 1, 1);
    GtkWidget* fullname_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(fullname_entry), "Enter full name");
    gtk_grid_attach(GTK_GRID(grid), fullname_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Email:"), 0, row, 1, 1);
    GtkWidget* email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "Enter email");
    gtk_grid_attach(GTK_GRID(grid), email_entry, 1, row++, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Role:"), 0, row, 1, 1);
    GtkWidget* role_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Administrator");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Doctor");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Nurse");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Receptionist");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Pharmacist");
    gtk_combo_box_set_active(GTK_COMBO_BOX(role_combo), 1); // Default to Doctor
    gtk_grid_attach(GTK_GRID(grid), role_combo, 1, row++, 1, 1);
    
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), button_box, FALSE, FALSE, 10);
    
    GtkWidget* save_button = gtk_button_new_with_label("💾 Create User");
    GtkWidget* cancel_button = gtk_button_new_with_label("❌ Cancel");
    
    gtk_box_pack_end(GTK_BOX(button_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(button_box), cancel_button, FALSE, FALSE, 0);
    
    // Store widgets for callback
    static GtkWidget* widgets[6];
    widgets[0] = username_entry;
    widgets[1] = password_entry;
    widgets[2] = fullname_entry;
    widgets[3] = email_entry;
    widgets[4] = role_combo;
    widgets[5] = dialog;
    
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_user_clicked), widgets);
    g_signal_connect_swapped(cancel_button, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
    
    gtk_widget_show_all(dialog);
}

static void on_save_user_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    GtkWidget** widgets = (GtkWidget**)data;
    
    User new_user;
    memset(&new_user, 0, sizeof(User));  // Initialize all fields to 0
    strcpy(new_user.username, gtk_entry_get_text(GTK_ENTRY(widgets[0])));
    strcpy(new_user.password, gtk_entry_get_text(GTK_ENTRY(widgets[1])));
    strcpy(new_user.full_name, gtk_entry_get_text(GTK_ENTRY(widgets[2])));
    strcpy(new_user.email, gtk_entry_get_text(GTK_ENTRY(widgets[3])));
    new_user.role = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets[4]));
    new_user.is_active = 1;
    new_user.doctor_id = 0;  // No doctor link by default
    
    if (strlen(new_user.username) == 0 || strlen(new_user.password) == 0) {
        show_error_dialog(GTK_WINDOW(widgets[5]), "Username and password are required!");
        return;
    }
    
    if (create_user(&new_user)) {
        gtk_widget_destroy(widgets[5]);
    } else {
        show_error_dialog(GTK_WINDOW(widgets[5]), "Failed to create user. Username may already exist.");
    }
}

void show_system_settings_dialog(GtkWidget* parent_window) {
    show_info_dialog(GTK_WINDOW(parent_window), "System Settings Dialog");
}

void generate_patient_report() { printf("Generating patient report...\n"); }
void generate_financial_report() { printf("Generating financial report...\n"); }
void backup_database() { printf("Backing up database...\n"); }
void restore_database() { printf("Restoring database...\n"); }
