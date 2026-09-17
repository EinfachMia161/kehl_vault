/**
 * @file audit.h
 * @brief Vault security analysis, health score evaluation, and duplicate detection.
 *
 * Performs static security audits across all entries in the vault to identify
 * weak, short, or reused passwords and calculate a comprehensive health score.
 */

#ifndef KEHL_VAULT_AUDIT_H
#define KEHL_VAULT_AUDIT_H

#include "entry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_REUSED_GROUPS 32

/**
 * @brief Represents a set of entries that share identical passwords.
 */
typedef struct {
    char password[ENTRY_PASSWORD_SIZE];
    int count;
    int entry_indices[32];
} ReusedPasswordGroup;

/**
 * @brief Comprehensive report of vault security metrics.
 */
typedef struct {
    int total_entries;
    int strong_passwords;
    int medium_passwords;
    int weak_passwords;
    int short_passwords;
    int reused_groups_count;
    ReusedPasswordGroup reused_groups[MAX_REUSED_GROUPS];
    int health_score; /* 0 to 100 percentage */
} VaultAuditReport;

/**
 * @brief Evaluates an EntryList and populates a security audit report.
 * @param list Pointer to EntryList to analyze.
 * @param report Pointer to VaultAuditReport to populate.
 * @return 1 on success, 0 on invalid parameters.
 */
int audit_vault(const EntryList* list, VaultAuditReport* report);

/**
 * @brief Formats and prints the audit report to standard output.
 * @param report Pointer to VaultAuditReport.
 */
void audit_print_report(const VaultAuditReport* report);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_AUDIT_H */
