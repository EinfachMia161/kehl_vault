/**
 * @file impex.h
 * @brief Data import and export subsystem (CSV and JSON formats).
 *
 * Implements RFC 4180-compliant CSV import/export with proper quote escaping,
 * and structured JSON serialization for interoperability and backups.
 */

#ifndef KEHL_VAULT_IMPEX_H
#define KEHL_VAULT_IMPEX_H

#include "entry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Exports an EntryList to an RFC 4180-compliant CSV file.
 * @param list Pointer to EntryList.
 * @param filepath Destination CSV file path.
 * @return 1 on success, 0 on failure.
 */
int impex_export_csv(const EntryList* list, const char* filepath);

/**
 * @brief Imports entries from a CSV file into an EntryList.
 * @param list Pointer to EntryList to receive imported items.
 * @param filepath Source CSV file path.
 * @return Number of imported entries (>= 0), or -1 on file error.
 */
int impex_import_csv(EntryList* list, const char* filepath);

/**
 * @brief Exports an EntryList to a structured JSON file.
 * @param list Pointer to EntryList.
 * @param filepath Destination JSON file path.
 * @return 1 on success, 0 on failure.
 */
int impex_export_json(const EntryList* list, const char* filepath);

/**
 * @brief Imports entries from a JSON file into an EntryList.
 * @param list Pointer to EntryList.
 * @param filepath Source JSON file path.
 * @return Number of imported entries (>= 0), or -1 on error.
 */
int impex_import_json(EntryList* list, const char* filepath);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_IMPEX_H */
