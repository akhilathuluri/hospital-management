#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Common constants
#define MAX_STRING 100
#define MAX_TEXT 500
#define MAX_PHONE 15
#define MAX_EMAIL 50
#define MAX_PASSWORD 64

// Date structure
typedef struct {
    int day;
    int month;
    int year;
} Date;

// Time structure
typedef struct {
    int hour;
    int minute;
} Time;

// Function declarations
void clear_screen();
char* trim_whitespace(char* str);
int is_valid_email(const char* email);
int is_valid_phone(const char* phone);
int is_valid_date(Date date);
int compare_dates(Date d1, Date d2);
void get_current_date(Date* date);
void get_current_time(Time* time);
char* date_to_string(Date date);
char* time_to_string(Time time);
Date string_to_date(const char* str);
Time string_to_time(const char* str);
void show_error_dialog(GtkWindow* parent, const char* message);
void show_info_dialog(GtkWindow* parent, const char* message);
int show_confirm_dialog(GtkWindow* parent, const char* message);
void hash_password(const char* password, char* hashed);

#endif // UTILS_H
