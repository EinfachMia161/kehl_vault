/**
 * @file profile.c
 * @brief Implementation of multi-profile vault database management.
 *
 * Implements discovery, switching, and naming schemes for multi-database workflows.
 */

#include "profile.h"
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

void profile_list_init(ProfileList* list) {
    if (!list) return;
    memset(list, 0, sizeof(ProfileList));
    list->active_index = -1;
}

int profile_get_filepath(const char* profile_name, char* output, size_t output_size) {
    if (!profile_name || !output || output_size == 0) {
        return 0;
    }

    if (strcmp(profile_name, "default") == 0 || strcmp(profile_name, "main") == 0) {
        int written = snprintf(output, output_size, "vault.dat");
        return (written > 0 && (size_t)written < output_size);
    }

    int written = snprintf(output, output_size, "vault_%s.dat", profile_name);
    return (written > 0 && (size_t)written < output_size);
}

int profile_list_add(ProfileList* list, const char* name, const char* filepath, int is_default) {
    if (!list || !name || !filepath || list->count >= MAX_PROFILES) {
        return 0;
    }

    /* Check if already exists */
    for (int i = 0; i < list->count; ++i) {
        if (strcmp(list->profiles[i].name, name) == 0) {
            return 0;
        }
    }

    VaultProfile* prof = &list->profiles[list->count];
    strncpy(prof->name, name, sizeof(prof->name) - 1);
    prof->name[sizeof(prof->name) - 1] = '\0';

    strncpy(prof->filepath, filepath, sizeof(prof->filepath) - 1);
    prof->filepath[sizeof(prof->filepath) - 1] = '\0';

    prof->is_default = is_default;

    if (list->active_index == -1 || is_default) {
        list->active_index = list->count;
    }

    list->count++;
    return 1;
}

int profile_discover(ProfileList* list) {
    if (!list) return 0;
    profile_list_init(list);

    /* Always ensure default profile exists in list */
    profile_list_add(list, "default", "vault.dat", 1);

#if defined(_WIN32) || defined(_WIN64)
    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA("vault_*.dat", &find_data);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            /* Extract profile name from vault_<name>.dat */
            const char* filename = find_data.cFileName;
            const char* prefix = "vault_";
            const char* suffix = ".dat";
            size_t prefix_len = strlen(prefix);
            size_t suffix_len = strlen(suffix);
            size_t filename_len = strlen(filename);

            if (filename_len > prefix_len + suffix_len) {
                char prof_name[PROFILE_NAME_MAX];
                size_t name_len = filename_len - prefix_len - suffix_len;
                if (name_len < sizeof(prof_name)) {
                    memcpy(prof_name, filename + prefix_len, name_len);
                    prof_name[name_len] = '\0';
                    profile_list_add(list, prof_name, filename, 0);
                }
            }
        } while (FindNextFileA(hFind, &find_data));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(".");
    if (dir != NULL) {
        struct dirent* ent;
        while ((ent = readdir(dir)) != NULL) {
            const char* filename = ent->d_name;
            const char* prefix = "vault_";
            const char* suffix = ".dat";
            size_t prefix_len = strlen(prefix);
            size_t suffix_len = strlen(suffix);
            size_t filename_len = strlen(filename);

            if (strncmp(filename, prefix, prefix_len) == 0 &&
                filename_len > prefix_len + suffix_len &&
                strcmp(filename + filename_len - suffix_len, suffix) == 0) {
                char prof_name[PROFILE_NAME_MAX];
                size_t name_len = filename_len - prefix_len - suffix_len;
                if (name_len < sizeof(prof_name)) {
                    memcpy(prof_name, filename + prefix_len, name_len);
                    prof_name[name_len] = '\0';
                    profile_list_add(list, prof_name, filename, 0);
                }
            }
        }
        closedir(dir);
    }
#endif

    return list->count;
}

int profile_set_active(ProfileList* list, int index) {
    if (!list || index < 0 || index >= list->count) {
        return 0;
    }
    list->active_index = index;
    return 1;
}

const VaultProfile* profile_get_active(const ProfileList* list) {
    if (!list || list->active_index < 0 || list->active_index >= list->count) {
        return NULL;
    }
    return &list->profiles[list->active_index];
}
