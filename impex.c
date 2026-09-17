#include "impex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* --- CSV Helpers --- */

static void csv_write_escaped_field(FILE* file, const char* field) {
    int needs_quotes = 0;
    for (int i = 0; field[i] != '\0'; ++i) {
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
    for (int i = 0; field[i] != '\0'; ++i) {
        if (field[i] == '"') {
            fputc('"', file);
            fputc('"', file);
        } else {
            fputc(field[i], file);
        }
    }
    fputc('"', file);
}

int vault_export_csv(const EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) {
        return 0;
    }

    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        return 0;
    }

    fputs("title,username,password\n", file);

    for (int i = 0; i < list->count; ++i) {
        const Entry* entry = &list->entries[i];
        csv_write_escaped_field(file, entry->title);
        fputc(',', file);
        csv_write_escaped_field(file, entry->username);
        fputc(',', file);
        csv_write_escaped_field(file, entry->password);
        fputc('\n', file);
    }

    fclose(file);
    return 1;
}

static const char* parse_csv_cell(const char* ptr, char* out, size_t out_size) {
    if (out_size == 0) return ptr;
    out[0] = '\0';
    size_t out_idx = 0;

    while (*ptr == ' ' || *ptr == '\t') ptr++;

    if (*ptr == '"') {
        ptr++; // skip opening quote
        while (*ptr != '\0') {
            if (*ptr == '"') {
                if (*(ptr + 1) == '"') {
                    if (out_idx + 1 < out_size) out[out_idx++] = '"';
                    ptr += 2;
                } else {
                    ptr++; // skip closing quote
                    break;
                }
            } else {
                if (out_idx + 1 < out_size) out[out_idx++] = *ptr;
                ptr++;
            }
        }
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == ',') ptr++;
    } else {
        while (*ptr != '\0' && *ptr != ',' && *ptr != '\n' && *ptr != '\r') {
            if (out_idx + 1 < out_size) out[out_idx++] = *ptr;
            ptr++;
        }
        if (*ptr == ',') ptr++;
    }

    out[out_idx] = '\0';
    return ptr;
}

int vault_import_csv(EntryList* list, const char* filepath, int* imported_count) {
    if (list == NULL || filepath == NULL) {
        return 0;
    }

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        return 0;
    }

    char line[2048];
    int count = 0;
    int is_first_line = 1;

    while (fgets(line, sizeof(line), file) != NULL) {
        // Strip trailing cr/lf
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n')) {
            line[--len] = '\0';
        }
        if (len == 0) continue;

        if (is_first_line) {
            is_first_line = 0;
            // Check for CSV header
            if (strncmp(line, "title,", 6) == 0 || strncmp(line, "Title,", 6) == 0 ||
                strncmp(line, "\"title\",", 8) == 0 || strncmp(line, "\"Title\",", 8) == 0) {
                continue;
            }
        }

        char title[ENTRY_TITLE_SIZE];
        char username[ENTRY_USERNAME_SIZE];
        char password[ENTRY_PASSWORD_SIZE];

        const char* p = line;
        p = parse_csv_cell(p, title, sizeof(title));
        p = parse_csv_cell(p, username, sizeof(username));
        p = parse_csv_cell(p, password, sizeof(password));

        if (strlen(title) > 0 || strlen(username) > 0 || strlen(password) > 0) {
            if (entry_list_add(list, title, username, password)) {
                count++;
            }
        }
    }

    fclose(file);
    if (imported_count != NULL) {
        *imported_count = count;
    }
    return 1;
}

/* --- JSON Helpers --- */

static void json_write_escaped_string(FILE* file, const char* str) {
    fputc('"', file);
    for (int i = 0; str[i] != '\0'; ++i) {
        char c = str[i];
        switch (c) {
            case '"':  fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\b': fputs("\\b", file); break;
            case '\f': fputs("\\f", file); break;
            case '\n': fputs("\\n", file); break;
            case '\r': fputs("\\r", file); break;
            case '\t': fputs("\\t", file); break;
            default:   fputc(c, file); break;
        }
    }
    fputc('"', file);
}

int vault_export_json(const EntryList* list, const char* filepath) {
    if (list == NULL || filepath == NULL) {
        return 0;
    }

    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        return 0;
    }

    fputs("[\n", file);
    for (int i = 0; i < list->count; ++i) {
        const Entry* entry = &list->entries[i];
        fputs("  {\n", file);

        fputs("    \"title\": ", file);
        json_write_escaped_string(file, entry->title);
        fputs(",\n", file);

        fputs("    \"username\": ", file);
        json_write_escaped_string(file, entry->username);
        fputs(",\n", file);

        fputs("    \"password\": ", file);
        json_write_escaped_string(file, entry->password);
        fputs("\n", file);

        if (i + 1 < list->count) {
            fputs("  },\n", file);
        } else {
            fputs("  }\n", file);
        }
    }
    fputs("]\n", file);

    fclose(file);
    return 1;
}

static const char* json_extract_string_value(const char* json_obj, const char* key, char* out, size_t out_size) {
    if (out_size == 0) return NULL;
    out[0] = '\0';

    char search_key[128];
    snprintf(search_key, sizeof(search_key), "\"%s\"", key);

    const char* p = strstr(json_obj, search_key);
    if (!p) return NULL;

    p += strlen(search_key);
    while (*p && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ':')) p++;

    if (*p != '"') return NULL;
    p++; // skip opening quote

    size_t out_idx = 0;
    while (*p && *p != '"') {
        if (*p == '\\') {
            p++;
            if (*p == '"') { if (out_idx + 1 < out_size) out[out_idx++] = '"'; }
            else if (*p == '\\') { if (out_idx + 1 < out_size) out[out_idx++] = '\\'; }
            else if (*p == 'n') { if (out_idx + 1 < out_size) out[out_idx++] = '\n'; }
            else if (*p == 'r') { if (out_idx + 1 < out_size) out[out_idx++] = '\r'; }
            else if (*p == 't') { if (out_idx + 1 < out_size) out[out_idx++] = '\t'; }
            else if (*p) { if (out_idx + 1 < out_size) out[out_idx++] = *p; }
        } else {
            if (out_idx + 1 < out_size) out[out_idx++] = *p;
        }
        if (*p) p++;
    }

    out[out_idx] = '\0';
    return p;
}

int vault_import_json(EntryList* list, const char* filepath, int* imported_count) {
    if (list == NULL || filepath == NULL) {
        return 0;
    }

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0 || file_size > 10 * 1024 * 1024) { // max 10MB
        fclose(file);
        return 0;
    }

    char* buffer = (char*)malloc((size_t)file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return 0;
    }

    size_t read_bytes = fread(buffer, 1, (size_t)file_size, file);
    fclose(file);
    buffer[read_bytes] = '\0';

    int count = 0;
    const char* p = buffer;

    while ((p = strchr(p, '{')) != NULL) {
        const char* obj_end = strchr(p, '}');
        if (!obj_end) break;

        char title[ENTRY_TITLE_SIZE] = {0};
        char username[ENTRY_USERNAME_SIZE] = {0};
        char password[ENTRY_PASSWORD_SIZE] = {0};

        json_extract_string_value(p, "title", title, sizeof(title));
        json_extract_string_value(p, "username", username, sizeof(username));
        json_extract_string_value(p, "password", password, sizeof(password));

        if (strlen(title) > 0 || strlen(username) > 0 || strlen(password) > 0) {
            if (entry_list_add(list, title, username, password)) {
                count++;
            }
        }

        p = obj_end + 1;
    }

    free(buffer);
    if (imported_count != NULL) {
        *imported_count = count;
    }
    return 1;
}
