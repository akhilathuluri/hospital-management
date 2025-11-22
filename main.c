#include "auth.h"
#include "patient.h"
#include "doctor.h"
#include "appointment.h"
#include "admission.h"
#include "admin_panel.h"
#include "doctor_panel.h"

static GtkWidget* main_window = NULL;
static GtkWidget* content_area = NULL;

typedef enum {
    NAV_DASHBOARD, NAV_PATIENTS, NAV_DOCTORS, NAV_APPOINTMENTS, NAV_ADMISSIONS,
    NAV_BILLING, NAV_PHARMACY, NAV_STAFF, NAV_USERS, NAV_SETTINGS
} NavigationItem;

static void on_nav_item_clicked(GtkWidget* widget, gpointer data) {
    NavigationItem item = GPOINTER_TO_INT(data);
    if (content_area) {
        GList* children = gtk_container_get_children(GTK_CONTAINER(content_area));
        for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);
    }
    
    GtkWidget* new_content = NULL;
    switch (item) {
        case NAV_DASHBOARD:
            if (g_session.current_user.role == ROLE_DOCTOR) {
                new_content = create_doctor_dashboard(main_window);
            } else {
                new_content = create_admin_dashboard(main_window);
            }
            break;
        case NAV_PATIENTS:
            if (g_session.current_user.role == ROLE_DOCTOR) {
                new_content = create_doctor_patients_view(main_window);
            } else {
                new_content = create_patient_list_view(main_window);
            }
            break;
        case NAV_DOCTORS:
            new_content = create_doctor_list_view(main_window);
            break;
        case NAV_APPOINTMENTS:
            if (g_session.current_user.role == ROLE_DOCTOR) {
                new_content = create_doctor_appointments_view(main_window);
            } else {
                new_content = create_appointment_view(main_window);
            }
            break;
        case NAV_ADMISSIONS:
            if (g_session.current_user.role == ROLE_DOCTOR) {
                new_content = create_doctor_admissions_view(main_window);
            } else {
                new_content = create_admission_view(main_window);
            }
            break;
        case NAV_USERS:
            if (g_session.current_user.role == ROLE_ADMIN) {
                new_content = create_user_management_view(main_window);
            } else {
                show_error_dialog(GTK_WINDOW(main_window), "Access denied. Admin privileges required.");
                return;
            }
            break;
        default:
            new_content = gtk_label_new("Feature coming soon...");
            break;
    }
    
    if (new_content) {
        gtk_box_pack_start(GTK_BOX(content_area), new_content, TRUE, TRUE, 0);
        gtk_widget_show_all(content_area);
    }
}

static void on_logout_clicked(GtkWidget* widget, gpointer data) {
    if (show_confirm_dialog(GTK_WINDOW(main_window), "Are you sure you want to logout?")) {
        logout_user();
        gtk_widget_destroy(main_window);
        show_login_window();
    }
}

void create_main_window_after_login() {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Hospital Management System");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1200, 700);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(main_window), hbox);
    
    GtkWidget* sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(sidebar, 250, -1);
    gtk_box_pack_start(GTK_BOX(hbox), sidebar, FALSE, FALSE, 0);
    
    GtkWidget* header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(header_box), 20);
    gtk_box_pack_start(GTK_BOX(sidebar), header_box, FALSE, FALSE, 0);
    
    GtkWidget* app_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(app_title), "<span size='large' weight='bold'>HMS</span>");
    gtk_box_pack_start(GTK_BOX(header_box), app_title, FALSE, FALSE, 0);
    
    GtkWidget* user_label = gtk_label_new(g_session.current_user.full_name);
    gtk_box_pack_start(GTK_BOX(header_box), user_label, FALSE, FALSE, 0);
    
    GtkWidget* role_label = gtk_label_new(get_role_name(g_session.current_user.role));
    gtk_box_pack_start(GTK_BOX(header_box), role_label, FALSE, FALSE, 0);
    
    GtkWidget* separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(sidebar), separator1, FALSE, FALSE, 10);
    
    const char* nav_labels[] = {
        "Dashboard", "Patients", "Doctors", "Appointments", "Admissions",
        "Billing", "Pharmacy", "Staff", "Users", "Settings"
    };
    
    for (int i = 0; i < 10; i++) {
        if (i == NAV_USERS && g_session.current_user.role != ROLE_ADMIN) continue;
        GtkWidget* nav_button = gtk_button_new_with_label(nav_labels[i]);
        gtk_button_set_relief(GTK_BUTTON(nav_button), GTK_RELIEF_NONE);
        gtk_widget_set_halign(nav_button, GTK_ALIGN_FILL);
        g_signal_connect(nav_button, "clicked", G_CALLBACK(on_nav_item_clicked), GINT_TO_POINTER(i));
        gtk_box_pack_start(GTK_BOX(sidebar), nav_button, FALSE, FALSE, 0);
    }
    
    GtkWidget* spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), spacer, TRUE, TRUE, 0);
    
    GtkWidget* separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(sidebar), separator2, FALSE, FALSE, 10);
    
    GtkWidget* logout_button = gtk_button_new_with_label("Logout");
    gtk_button_set_relief(GTK_BUTTON(logout_button), GTK_RELIEF_NONE);
    g_signal_connect(logout_button, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(sidebar), logout_button, FALSE, FALSE, 0);
    
    content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 20);
    gtk_box_pack_start(GTK_BOX(hbox), content_area, TRUE, TRUE, 0);
    
    GtkWidget* dashboard;
    if (g_session.current_user.role == ROLE_DOCTOR) {
        dashboard = create_doctor_dashboard(main_window);
    } else {
        dashboard = create_admin_dashboard(main_window);
    }
    gtk_box_pack_start(GTK_BOX(content_area), dashboard, TRUE, TRUE, 0);
    
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(main_window);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    init_auth_system();
    init_patient_system();
    init_doctor_system();
    init_appointment_system();
    init_admission_system();
    init_admin_panel();
    show_login_window();
    gtk_main();
    return 0;
}
