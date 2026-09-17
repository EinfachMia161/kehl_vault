/**
 * @file audit.c
 * @brief Implementation of vault security analysis and duplicate password detection.
 */

#include "audit.h"
#include "password.h"
#include <stdio.h>
#include <string.h>

int audit_vault(const EntryList* list, VaultAuditReport* report) {
    if (list == NULL || report == NULL) {
        return 0;
    }

    memset(report, 0, sizeof(VaultAuditReport));
    report->total_entries = list->count;

    if (list->count == 0) {
        report->health_score = 100;
        return 1;
    }

    for (int i = 0; i < list->count; ++i) {
        const Entry* e = &list->entries[i];
        int score = password_calculate_strength(e->password, 8);

        if (strlen(e->password) < 8) {
            report->short_passwords++;
        }

        if (score >= 4) {
            report->strong_passwords++;
        } else if (score == 3) {
            report->medium_passwords++;
        } else {
            report->weak_passwords++;
        }

        /* Check for duplicate passwords across other entries */
        int found_group = -1;
        for (int g = 0; g < report->reused_groups_count; ++g) {
            if (strcmp(report->reused_groups[g].password, e->password) == 0) {
                found_group = g;
                break;
            }
        }

        if (found_group >= 0) {
            ReusedPasswordGroup* grp = &report->reused_groups[found_group];
            if (grp->count < 32) {
                grp->entry_indices[grp->count] = i;
                grp->count++;
            }
        } else {
            /* Check if any previous entry matches */
            int matches = 0;
            int first_match_idx = -1;
            for (int prev = 0; prev < i; ++prev) {
                if (strcmp(list->entries[prev].password, e->password) == 0) {
                    matches++;
                    if (first_match_idx == -1) first_match_idx = prev;
                }
            }

            if (matches > 0 && report->reused_groups_count < MAX_REUSED_GROUPS) {
                ReusedPasswordGroup* grp = &report->reused_groups[report->reused_groups_count];
                strncpy(grp->password, e->password, sizeof(grp->password) - 1);
                grp->password[sizeof(grp->password) - 1] = '\0';
                grp->entry_indices[0] = first_match_idx;
                grp->entry_indices[1] = i;
                grp->count = 2;
                report->reused_groups_count++;
            }
        }
    }

    /* Health Score Calculation (0-100) */
    int total = list->count;
    int base_points = (report->strong_passwords * 100 + report->medium_passwords * 60) / total;

    int penalty = 0;
    penalty += (report->weak_passwords * 20) / total;
    penalty += (report->short_passwords * 25) / total;
    penalty += (report->reused_groups_count * 15);

    int final_score = base_points - penalty;
    if (final_score < 0) final_score = 0;
    if (final_score > 100) final_score = 100;

    report->health_score = final_score;
    return 1;
}

void audit_print_report(const VaultAuditReport* report) {
    if (report == NULL) return;

    printf("\n========================================\n");
    printf("        VAULT SECURITY AUDIT            \n");
    printf("========================================\n");
    printf("Total Entries:      %d\n", report->total_entries);
    printf("Strong Passwords:   %d\n", report->strong_passwords);
    printf("Medium Passwords:   %d\n", report->medium_passwords);
    printf("Weak Passwords:     %d\n", report->weak_passwords);
    printf("Short (<8 chars):   %d\n", report->short_passwords);
    printf("Reused Passwords:   %d group(s)\n", report->reused_groups_count);
    printf("----------------------------------------\n");
    printf("Overall Vault Health: %d%%\n", report->health_score);
    printf("========================================\n");
}
