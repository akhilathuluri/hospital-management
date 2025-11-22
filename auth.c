#include "auth.h"
#include <string.h>

#define MAX_USERS 100
#define USERS_FILE "data/users.dat"

Session g_session = {0};
static User users[MAX_USERS];
static int user_count = 0;

extern void create_main_window_after_login();

void init_auth_system() {
    g_session.is_logged_in = 0;
    load_users_from_file();
    if (user_count == 0) create_default_admin();
}

void create_default_admin() {
    User admin;
    admin.id = 1;
    strcpy(admin.username, "admin");
    strcpy(admin.full_name, "System Administrator");
    strcpy(admin.email, "admin@hospital.com");
    admin.role = ROLE_ADMIN;
    admin.is_active = 1;
    get_current_date(&admin.created_date);
    hash_password("admin123", admin.password);
    users[user_count++] = admin;
    save_users_to_file();
    printf("Default admin created - Username: admin, Password: admin123\n");
}

int authenticate_user(const char* username, const char* password) {
    char hashed_password[MAX_PASSWORD];
    hash_password(password, hashed_password);
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0 && 
            strcmp(users[i].password, hashed_password) == 0 && users[i].is_active) {
            g_session.current_user = users[i];
            g_session.is_logged_in = 1;
            return 1;
        }
    }
    return 0;
}

void logout_user() {
    g_session.is_logged_in = 0;
    memset(&g_session.current_user, 0, sizeof(User));
}

int create_user(User* user) {
    if (user_count >= MAX_USERS) return 0;
    if (get_user_by_username(user->username) != NULL) return 0;
    user->id = user_count + 1;
    get_current_date(&user->created_date);
    user->is_active = 1;
    if (user->doctor_id == 0) {
        user->doctor_id = 0;  // No doctor link by default
    }
    hash_password(user->password, user->password);
    users[user_count++] = *user;
    save_users_to_file();
    return 1;
}

int update_user(User* user) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == user->id) {
            if (strlen(user->password) == 0) {
                strcpy(user->password, users[i].password);
            } else {
                char temp_password[MAX_PASSWORD];
                strcpy(temp_password, user->password);
                hash_password(temp_password, user->password);
            }
            users[i] = *user;
            save_users_to_file();
            return 1;
        }
    }
    return 0;
}

int delete_user(int user_id) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == user_id) {
            for (int j = i; j < user_count - 1; j++) users[j] = users[j + 1];
            user_count--;
            save_users_to_file();
            return 1;
        }
    }
    return 0;
}

User* get_user_by_id(int user_id) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == user_id) return &users[i];
    }
    return NULL;
}

User* get_user_by_username(const char* username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) return &users[i];
    }
    return NULL;
}

int get_all_users(User** users_ptr) {
    *users_ptr = users;
    return user_count;
}

int has_permission(UserRole required_role) {
    if (!g_session.is_logged_in) return 0;
    if (g_session.current_user.role == ROLE_ADMIN) return 1;
    return (g_session.current_user.role == required_role);
}

const char* get_role_name(UserRole role) {
    switch (role) {
        case ROLE_ADMIN: return "Administrator";
        case ROLE_DOCTOR: return "Doctor";
        case ROLE_NURSE: return "Nurse";
        case ROLE_RECEPTIONIST: return "Receptionist";
        case ROLE_PHARMACIST: return "Pharmacist";
        default: return "Unknown";
    }
}

void save_users_to_file() {
    FILE* file = fopen(USERS_FILE, "wb");
    if (!file) {
        #ifdef _WIN32
            system("mkdir data 2>nul");
        #else
            system("mkdir -p data");
        #endif
        file = fopen(USERS_FILE, "wb");
        if (!file) { printf("Error: Cannot create users file\n"); return; }
    }
    fwrite(&user_count, sizeof(int), 1, file);
    fwrite(users, sizeof(User), user_count, file);
    fclose(file);
}

void load_users_from_file() {
    FILE* file = fopen(USERS_FILE, "rb");
    if (!file) { user_count = 0; return; }
    fread(&user_count, sizeof(int), 1, file);
    fread(users, sizeof(User), user_count, file);
    fclose(file);
}

static void on_login_clicked(GtkWidget* widget, gpointer data) {
    (void)widget;
    GtkWidget** entries = (GtkWidget**)data;
    const char* username = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char* password = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    if (strlen(username) == 0 || strlen(password) == 0) {
        show_error_dialog(GTK_WINDOW(entries[2]), "Please enter both username and password");
        return;
    }
    if (authenticate_user(username, password)) {
        gtk_widget_destroy(entries[2]);
        create_main_window_after_login();
    } else {
        show_error_dialog(GTK_WINDOW(entries[2]), "Invalid username or password");
        gtk_entry_set_text(GTK_ENTRY(entries[1]), "");
    }
}

GtkWidget* create_login_window() {
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Hospital Management System - Login");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);
    
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    GtkWidget* title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='x-large' weight='bold'>Hospital Management System</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title_label, FALSE, FALSE, 10);
    
    GtkWidget* subtitle_label = gtk_label_new("Please login to continue");
    gtk_box_pack_start(GTK_BOX(vbox), subtitle_label, FALSE, FALSE, 5);
    
    GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 10);
    
    GtkWidget* username_label = gtk_label_new("Username:");
    gtk_widget_set_halign(username_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), username_label, FALSE, FALSE, 0);
    
    GtkWidget* username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
    gtk_box_pack_start(GTK_BOX(vbox), username_entry, FALSE, FALSE, 0);
    
    GtkWidget* password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(vbox), password_label, FALSE, FALSE, 0);
    
    GtkWidget* password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_box_pack_start(GTK_BOX(vbox), password_entry, FALSE, FALSE, 0);
    
    GtkWidget* login_button = gtk_button_new_with_label("Login");
    gtk_box_pack_start(GTK_BOX(vbox), login_button, FALSE, FALSE, 10);
    
    static GtkWidget* entries[3];
    entries[0] = username_entry;
    entries[1] = password_entry;
    entries[2] = window;
    
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_clicked), entries);
    // Don't connect destroy to gtk_main_quit - let main window handle that
    
    GtkWidget* info_label = gtk_label_new("Default login: admin / admin123");
    gtk_widget_set_halign(info_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), info_label, FALSE, FALSE, 5);
    
    return window;
}

void show_login_window() {
    GtkWidget* login_window = create_login_window();
    gtk_widget_show_all(login_window);
}
