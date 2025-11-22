#include "utils.h"
#include <ctype.h>

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

char* trim_whitespace(char* str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int is_valid_email(const char* email) {
    int at_count = 0;
    int dot_after_at = 0;
    const char* at_pos = NULL;
    for (const char* p = email; *p; p++) {
        if (*p == '@') { at_count++; at_pos = p; }
        if (at_pos && *p == '.') dot_after_at = 1;
    }
    return (at_count == 1 && dot_after_at && strlen(email) > 5);
}

int is_valid_phone(const char* phone) {
    int len = strlen(phone);
    if (len < 10 || len > 15) return 0;
    for (int i = 0; i < len; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+' && phone[i] != '-' && phone[i] != ' ') return 0;
    }
    return 1;
}

int is_valid_date(Date date) {
    if (date.year < 1900 || date.year > 2100) return 0;
    if (date.month < 1 || date.month > 12) return 0;
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0)) days_in_month[1] = 29;
    if (date.day < 1 || date.day > days_in_month[date.month - 1]) return 0;
    return 1;
}

int compare_dates(Date d1, Date d2) {
    if (d1.year != d2.year) return (d1.year > d2.year) ? 1 : -1;
    if (d1.month != d2.month) return (d1.month > d2.month) ? 1 : -1;
    if (d1.day != d2.day) return (d1.day > d2.day) ? 1 : -1;
    return 0;
}

void get_current_date(Date* date) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    date->day = tm_info->tm_mday;
    date->month = tm_info->tm_mon + 1;
    date->year = tm_info->tm_year + 1900;
}

void get_current_time(Time* time_struct) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    time_struct->hour = tm_info->tm_hour;
    time_struct->minute = tm_info->tm_min;
}

char* date_to_string(Date date) {
    static char buffer[12];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", date.day, date.month, date.year);
    return buffer;
}

char* time_to_string(Time time_struct) {
    static char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", time_struct.hour, time_struct.minute);
    return buffer;
}

Date string_to_date(const char* str) {
    Date date = {0, 0, 0};
    sscanf(str, "%d/%d/%d", &date.day, &date.month, &date.year);
    return date;
}

Time string_to_time(const char* str) {
    Time time_struct = {0, 0};
    sscanf(str, "%d:%d", &time_struct.hour, &time_struct.minute);
    return time_struct;
}

void show_error_dialog(GtkWindow* parent, const char* message) {
    GtkWidget* dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_info_dialog(GtkWindow* parent, const char* message) {
    GtkWidget* dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int show_confirm_dialog(GtkWindow* parent, const char* message) {
    GtkWidget* dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", message);
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return (result == GTK_RESPONSE_YES);
}

void hash_password(const char* password, char* hashed) {
    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        hashed[i] = password[i] ^ 0x5A;
    }
    hashed[len] = '\0';
}
