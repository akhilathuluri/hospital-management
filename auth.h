#ifndef AUTH_H
#define AUTH_H

#include "utils.h"

typedef enum {
    ROLE_ADMIN,
    ROLE_DOCTOR,
    ROLE_NURSE,
    ROLE_RECEPTIONIST,
    ROLE_PHARMACIST
} UserRole;

typedef struct {
    int id;
    char username[MAX_STRING];
    char password[MAX_PASSWORD];
    char full_name[MAX_STRING];
    char email[MAX_EMAIL];
    UserRole role;
    int is_active;
    int doctor_id;  // Links to doctor record if role is ROLE_DOCTOR
    Date created_date;
} User;

typedef struct {
    int is_logged_in;
    User current_user;
} Session;

extern Session g_session;

void init_auth_system();
int authenticate_user(const char* username, const char* password);
void logout_user();
int create_user(User* user);
int update_user(User* user);
int delete_user(int user_id);
User* get_user_by_id(int user_id);
User* get_user_by_username(const char* username);
int get_all_users(User** users);
int has_permission(UserRole required_role);
const char* get_role_name(UserRole role);
void save_users_to_file();
void load_users_from_file();
void create_default_admin();
GtkWidget* create_login_window();
void show_login_window();

#endif
