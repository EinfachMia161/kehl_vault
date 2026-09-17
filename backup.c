/**
 * @file backup.c
 * @brief Implementation of backup snapshot creation, listing, restoration, and pruning.
 */

#include "backup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static int copy_file_bytes(const char* src_path, const char* dst_path) {
    FILE* src = fopen(src_path, "rb");
    if (!src) return 0;

    FILE* dst = fopen(dst_path, "wb");
    if (!dst) {
        fclose(src);
        return 0;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            remove(dst_path);
            return 0;
        }
    }

    fclose(src);
    fclose(dst);
    return 1;
}

int backup_create_snapshot(
    const char* source_filepath,
    char* out_backup_path,
    size_t out_backup_path_size
) {
    if (!source_filepath) return 0;

    /* Verify source file exists */
    FILE* test = fopen(source_filepath, "rb");
    if (!test) return 0;
    fclose(test);

    uint64_t now = (uint64_t)time(NULL);
    char backup_filepath[BACKUP_PATH_MAX];
    int written = snprintf(backup_filepath, sizeof(backup_filepath), "%s.bak_%llu", source_filepath, (unsigned long long)now);
    if (written < 0 || (size_t)written >= sizeof(backup_filepath)) {
        return 0;
    }

    if (!copy_file_bytes(source_filepath, backup_filepath)) {
        return 0;
    }

    if (out_backup_path && out_backup_path_size > 0) {
        strncpy(out_backup_path, backup_filepath, out_backup_path_size - 1);
        out_backup_path[out_backup_path_size - 1] = '\0';
    }

    /* Automatically prune old snapshots */
    backup_prune_old_snapshots(source_filepath, MAX_BACKUPS_RETAINED);

    return 1;
}

int backup_list_snapshots(const char* source_filepath, BackupList* list) {
    if (!source_filepath || !list) return 0;
    memset(list, 0, sizeof(BackupList));

    char pattern[BACKUP_PATH_MAX];
    snprintf(pattern, sizeof(pattern), "%s.bak_*", source_filepath);

#if defined(_WIN32) || defined(_WIN64)
    WIN32_FIND_DATAA find_data;
    HANDLE hFind = FindFirstFileA(pattern, &find_data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (list->count < MAX_BACKUPS_RETAINED) {
                BackupSnapshot* snap = &list->snapshots[list->count];
                strncpy(snap->filepath, find_data.cFileName, sizeof(snap->filepath) - 1);
                snap->filepath[sizeof(snap->filepath) - 1] = '\0';

                snap->file_size = (size_t)find_data.nFileSizeLow;

                const char* underscore = strrchr(find_data.cFileName, '_');
                if (underscore) {
                    snap->timestamp = (uint64_t)strtoull(underscore + 1, NULL, 10);
                } else {
                    snap->timestamp = 0;
                }
                list->count++;
            }
        } while (FindNextFileA(hFind, &find_data));
        FindClose(hFind);
    }
#else
    DIR* dir = opendir(".");
    if (dir != NULL) {
        struct dirent* ent;
        char prefix[BACKUP_PATH_MAX];
        snprintf(prefix, sizeof(prefix), "%s.bak_", source_filepath);
        size_t prefix_len = strlen(prefix);

        while ((ent = readdir(dir)) != NULL) {
            if (strncmp(ent->d_name, prefix, prefix_len) == 0 && list->count < MAX_BACKUPS_RETAINED) {
                BackupSnapshot* snap = &list->snapshots[list->count];
                strncpy(snap->filepath, ent->d_name, sizeof(snap->filepath) - 1);
                snap->filepath[sizeof(snap->filepath) - 1] = '\0';

                const char* underscore = strrchr(ent->d_name, '_');
                if (underscore) {
                    snap->timestamp = (uint64_t)strtoull(underscore + 1, NULL, 10);
                }
                list->count++;
            }
        }
        closedir(dir);
    }
#endif

    return list->count;
}

int backup_restore_snapshot(
    const char* snapshot_filepath,
    const char* target_vault_filepath
) {
    if (!snapshot_filepath || !target_vault_filepath) return 0;
    return copy_file_bytes(snapshot_filepath, target_vault_filepath);
}

int backup_prune_old_snapshots(const char* source_filepath, int max_to_keep) {
    if (!source_filepath || max_to_keep <= 0) return 0;

    BackupList list;
    backup_list_snapshots(source_filepath, &list);

    if (list.count <= max_to_keep) {
        return 0;
    }

    int pruned = 0;
    int to_delete = list.count - max_to_keep;

    for (int i = 0; i < to_delete && i < list.count; ++i) {
        if (remove(list.snapshots[i].filepath) == 0) {
            pruned++;
        }
    }

    return pruned;
}
