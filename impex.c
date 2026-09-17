/**
 * @file impex.c
 * @brief Implementation of CSV and JSON serialization and parsing routines.
 */

#include "impex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void write_csv_field(FILE* file, const char* field) {
    int needs_quotes = 0;
    for (size_t i = 0; field[i] != '\0'; i++) {
        if (field[i] == ',' || field[i] == '"' || field[i] == '\n' || field[i] == '\r') {
            needs_quotes = 1;
            break;
        }
    }

    if (!needs_quotes) {
        fputs(field, file);
        return;
    }

    fputc('"', file);
    for (size_t i = 0; field[i] != '\0'; i++) {
        if (field[i] == '"') {
            fputc('"', file); /* Double quote escape */
        }
        fputc(field[i], file);
    }
    fputc('"', file);
}

int impex_export_csv(const EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) return 0;

    FILE* file = fopen(filepath, "w");
    if (file == NULL) return 0;

    /* Write CSV header */
    fprintf(file, "title,username,password\n");

    for (int i = 0; i < list->count; ++i) {
        write_csv_field(file, list->entries[i].title);
        fputc(',', file);
        write_csv_field(file, list->entries[i].username);
        fputc(',', file);
        write_csv_field(file, list->entries[i].password);
        fputc('\n', file);
    }

    fclose(file);
    return 1;
}

int impex_import_csv(EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) return -1;

    FILE* file = fopen(filepath, "r");
    if (file == NULL) return -1;

    char line[1024];
    int imported_count = 0;
    int is_header = 1;

    while (fgets(line, sizeof(line), file)) {
        /* Strip line endings */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n')) {
            line[--len] = '\0';
        }

        if (len == 0) continue;

        /* Skip header line */
        if (is_header) {
            is_header = 0;
            if (strstr(line, "title") != NULL || strstr(line, "username") != NULL) {
                continue;
            }
        }

        /* Simple CSV parser splitting by comma */
        char title[ENTRY_TITLE_SIZE] = {0};
        char username[ENTRY_USERNAME_SIZE] = {0};
        char password[ENTRY_PASSWORD_SIZE] = {0};

        char* p = line;
        char* tokens[3] = {title, username, password};
        size_t max_lens[3] = {sizeof(title), sizeof(username), sizeof(password)};

        for (int field_idx = 0; field_idx < 3; ++field_idx) {
            if (*p == '\0') break;

            if (*p == '"') {
                p++; /* Skip opening quote */
                size_t out_idx = 0;
                while (*p != '\0') {
                    if (*p == '"') {
                        if (*(p + 1) == '"') {
                            if (out_idx < max_lens[field_idx] - 1) tokens[field_idx][out_idx++] = '"';
                            p += 2;
                        } else {
                            p++; /* Closing quote */
                            break;
                        }
                    } else {
                        if (out_idx < max_lens[field_idx] - 1) tokens[field_idx][out_idx++] = *p;
                        p++;
                    }
                }
                tokens[field_idx][out_idx] = '\0';
                if (*p == ',') p++;
            } else {
                char* comma = strchr(p, ',');
                if (comma != NULL) {
                    *comma = '\0';
                    strncpy(tokens[field_idx], p, max_lens[field_idx] - 1);
                    p = comma + 1;
                } else {
                    strncpy(tokens[field_idx], p, max_lens[field_idx] - 1);
                    p += strlen(p);
                }
            }
        }

        if (strlen(title) > 0) {
            if (entry_list_add(list, title, username, password)) {
                imported_count++;
            }
        }
    }

    fclose(file);
    return imported_count;
}

static void write_json_escaped_string(FILE* file, const char* str) {
    fputc('"', file);
    for (size_t i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '"':  fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\b': fputs("\\b", file); break;
            case '\f': fputs("\\f", file); break;
            case '\n': fputs("\\n", file); break;
            case '\r': fputs("\\r", file); break;
            case '\t': fputs("\\t", file); break;
            default:   fputc(str[i], file); break;
        }
    }
    fputc('"', file);
}

int impex_export_json(const EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) return 0;

    FILE* file = fopen(filepath, "w");
    if (file == NULL) return 0;

    fprintf(file, "{\n  \"version\": 1,\n  \"entries\": [\n");

    for (int i = 0; i < list->count; ++i) {
        fprintf(file, "    {\n      \"title\": ");
        write_json_escaped_string(file, list->entries[i].title);
        fprintf(file, ",\n      \"username\": ");
        write_json_escaped_string(file, list->entries[i].username);
        fprintf(file, ",\n      \"password\": ");
        write_json_escaped_string(file, list->entries[i].password);
        fprintf(file, "\n    }%s\n", (i == list->count - 1) ? "" : ",");
    }

    fprintf(file, "  ]\n}\n");
    fclose(file);
    return 1;
}

int impex_import_json(EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) return -1;

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) return -1;

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fsize <= 0 || fsize > 10 * 1024 * 1024) {
        fclose(file);
        return -1;
    }

    char* buffer = (char*)malloc((size_t)fsize + 1);
    if (buffer == NULL) {
        fclose(file);
        return -1;
    }

    size_t read_bytes = fread(buffer, 1, (size_t)fsize, file);
    fclose(file);
    buffer[read_bytes] = '\0';

    int imported_count = 0;
    const char* ptr = buffer;

    while ((ptr = strstr(ptr, "\"title\"")) != NULL) {
        char title[ENTRY_TITLE_SIZE] = {0};
        char username[ENTRY_USERNAME_SIZE] = {0};
        char password[ENTRY_PASSWORD_SIZE] = {0};

        /* Extract title */
        const char* val_start = strchr(ptr, ':');
        if (val_start) {
            val_start = strchr(val_start, '"');
            if (val_start) {
                val_start++;
                const char* val_end = strchr(val_start, '"');
                if (val_end) {
                    size_t len = (size_t)(val_end - val_start);
                    if (len >= ENTRY_TITLE_SIZE) len = ENTRY_TITLE_SIZE - 1;
                    strncpy(title, val_start, len);
                    title[len] = '\0';
                }
            }
        }

        /* Extract username */
        const char* u_ptr = strstr(ptr, "\"username\"");
        if (u_ptr) {
            val_start = strchr(u_ptr, ':');
            if (val_start) {
                val_start = strchr(val_start, '"');
                if (val_start) {
                    val_start++;
                    const char* val_end = strchr(val_start, '"');
                    if (val_end) {
                        size_t len = (size_t)(val_end - val_start);
                        if (len >= ENTRY_USERNAME_SIZE) len = ENTRY_USERNAME_SIZE - 1;
                        strncpy(username, val_start, len);
                        username[len] = '\0';
                    }
                }
            }
        }

        /* Extract password */
        const char* p_ptr = strstr(ptr, "\"password\"");
        if (p_ptr) {
            val_start = strchr(p_ptr, ':');
            if (val_start) {
                val_start = strchr(val_start, '"');
                if (val_start) {
                    val_start++;
                    const char* val_end = strchr(val_start, '"');
                    if (val_end) {
                        size_t len = (size_t)(val_end - val_start);
                        if (len >= ENTRY_PASSWORD_SIZE) len = ENTRY_PASSWORD_SIZE - 1;
                        strncpy(password, val_start, len);
                        password[len] = '\0';
                    }
                }
            }
        }

        if (strlen(title) > 0) {
            if (entry_list_add(list, title, username, password)) {
                imported_count++;
            }
        }

        ptr += 7;
    }

    free(buffer);
    return imported_count;
}
