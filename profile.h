/**
 * @file profile.h
 * @brief Multi-profile and multi-vault database management for kehl-vault.
 *
 * Provides capabilities to create, list, switch, and remove independent
 * vault database files for different user contexts (e.g., Personal, Work, Finance).
 */

#ifndef KEHL_VAULT_PROFILE_H
#define KEHL_VAULT_PROFILE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROFILE_NAME_MAX 64
#define PROFILE_PATH_MAX 260
#define MAX_PROFILES 32

/**
 * @brief Represents a single vault profile with its display name and file path.
 */
typedef struct {
    char name[PROFILE_NAME_MAX];
    char filepath[PROFILE_PATH_MAX];
    int is_default;
} VaultProfile;

/**
 * @brief Collection of available vault profiles discovered or registered.
 */
typedef struct {
    VaultProfile profiles[MAX_PROFILES];
    int count;
    int active_index;
} ProfileList;

/**
 * @brief Initializes the profile list structure.
 * @param list Pointer to ProfileList to initialize.
 */
void profile_list_init(ProfileList* list);

/**
 * @brief Formats a standard file path for a named profile.
 * @param profile_name Name of the profile (e.g. "work", "finance").
 * @param output Buffer to receive the resulting file path.
 * @param output_size Size of the output buffer.
 * @return 1 on success, 0 on invalid parameters.
 */
int profile_get_filepath(const char* profile_name, char* output, size_t output_size);

/**
 * @brief Adds a new profile to the list.
 * @param list Pointer to ProfileList.
 * @param name Profile name.
 * @param filepath Corresponding database path.
 * @param is_default Flag indicating if this is the default profile.
 * @return 1 on success, 0 on failure (e.g. list full or invalid params).
 */
int profile_list_add(ProfileList* list, const char* name, const char* filepath, int is_default);

/**
 * @brief Discovers standard profiles in the current working directory.
 * @param list Pointer to ProfileList to populate.
 * @return Number of profiles discovered.
 */
int profile_discover(ProfileList* list);

/**
 * @brief Sets the active profile by index.
 * @param list Pointer to ProfileList.
 * @param index Profile index.
 * @return 1 on success, 0 if out of range.
 */
int profile_set_active(ProfileList* list, int index);

/**
 * @brief Gets the currently active profile.
 * @param list Pointer to ProfileList.
 * @return Pointer to active VaultProfile, or NULL if list is empty.
 */
const VaultProfile* profile_get_active(const ProfileList* list);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_PROFILE_H */
