/**
 * @file backup.h
 * @brief Encrypted vault snapshot and automated backup rotation subsystem for kehl-vault.
 *
 * Automatically creates timestamped snapshot backups of the encrypted vault database
 * prior to critical modifications, with configurable retention policies and restore mechanisms.
 */

#ifndef KEHL_VAULT_BACKUP_H
#define KEHL_VAULT_BACKUP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BACKUPS_RETAINED 10
#define BACKUP_PATH_MAX 300

/**
 * @brief Represents a single backup snapshot file.
 */
typedef struct {
    char filepath[BACKUP_PATH_MAX];
    uint64_t timestamp;
    size_t file_size;
} BackupSnapshot;

/**
 * @brief Collection of discovered backup snapshots.
 */
typedef struct {
    BackupSnapshot snapshots[MAX_BACKUPS_RETAINED];
    int count;
} BackupList;

/**
 * @brief Creates a new timestamped backup snapshot from the current database file.
 * @param source_filepath Path to the active vault file (e.g. "vault.dat").
 * @param out_backup_path Optional buffer to receive the path of the created backup file.
 * @param out_backup_path_size Capacity of out_backup_path buffer.
 * @return 1 on successful backup creation, 0 on failure.
 */
int backup_create_snapshot(
    const char* source_filepath,
    char* out_backup_path,
    size_t out_backup_path_size
);

/**
 * @brief Discovers and lists all backup snapshots associated with a vault file.
 * @param source_filepath Source vault file path.
 * @param list Pointer to BackupList to populate.
 * @return Number of snapshots found.
 */
int backup_list_snapshots(const char* source_filepath, BackupList* list);

/**
 * @brief Restores a vault database file from a selected backup snapshot file.
 * @param snapshot_filepath Path of the backup snapshot to restore.
 * @param target_vault_filepath Target active vault database path to overwrite.
 * @return 1 on success, 0 on failure.
 */
int backup_restore_snapshot(
    const char* snapshot_filepath,
    const char* target_vault_filepath
);

/**
 * @brief Prunes older backups to maintain at most max_to_keep snapshots.
 * @param source_filepath Source vault file path.
 * @param max_to_keep Maximum number of newest snapshots to retain.
 * @return Number of pruned/deleted old snapshot files.
 */
int backup_prune_old_snapshots(const char* source_filepath, int max_to_keep);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_BACKUP_H */
