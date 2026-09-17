#ifndef KEHL_VAULT_AUDIT_H
#define KEHL_VAULT_AUDIT_H

#include "entry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int total_entries;
    int weak_count;
    int medium_count;
    int strong_count;
    int short_count;
    int reused_count;
    int unique_passwords;
    int health_score; /* 0 - 100 */
} VaultAuditReport;

int vault_audit_analyze(const EntryList* list, VaultAuditReport* report);

#ifdef __cplusplus
}
#endif

#endif
