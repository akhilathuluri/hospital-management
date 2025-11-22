#ifndef ADMIN_PANEL_H
#define ADMIN_PANEL_H

#include "utils.h"
#include "auth.h"

typedef struct {
    int total_patients;
    int total_doctors;
    int total_appointments_today;
    int pending_appointments;
    double total_revenue;
    int active_users;
} DashboardStats;

void init_admin_panel();
DashboardStats get_dashboard_statistics();
void generate_patient_report();
void generate_financial_report();
void backup_database();
void restore_database();
GtkWidget* create_admin_dashboard(GtkWidget* parent_window);
GtkWidget* create_user_management_view(GtkWidget* parent_window);
void show_add_user_dialog(GtkWidget* parent_window);
void show_system_settings_dialog(GtkWidget* parent_window);

#endif
