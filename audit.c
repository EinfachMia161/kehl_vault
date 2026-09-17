#include "audit.h"
#include "password.h"
#include <string.h>

int vault_audit_analyze(const EntryList* list, VaultAuditReport* report) {
    if (list == NULL || report == NULL) {
        return 0;
    }

    memset(report, 0, sizeof(VaultAuditReport));
    report->total_entries = list->count;

    if (list->count == 0) {
        report->health_score = 100;
        return 1;
    }

    int total_score_sum = 0;

    for (int i = 0; i < list->count; ++i) {
        const Entry* entry = &list->entries[i];
        int strength = password_calculate_strength(entry->password, 8);
        total_score_sum += strength;

        if (strlen(entry->password) < 8) {
            report->short_count++;
        }

        if (strength <= 2) {
            report->weak_count++;
        } else if (strength <= 4) {
            report->medium_count++;
        } else {
            report->strong_count++;
        }

        int is_reused = 0;
        for (int j = 0; j < list->count; ++j) {
            if (i != j && strcmp(entry->password, list->entries[j].password) == 0) {
                is_reused = 1;
                break;
            }
        }

        if (is_reused) {
            report->reused_count++;
        }
    }

    /* Calculate unique passwords */
    int unique_count = 0;
    for (int i = 0; i < list->count; ++i) {
        int first_seen = 1;
        for (int j = 0; j < i; ++j) {
            if (strcmp(list->entries[i].password, list->entries[j].password) == 0) {
                first_seen = 0;
                break;
            }
        }
        if (first_seen) {
            unique_count++;
        }
    }
    report->unique_passwords = unique_count;

    /* Health calculation: base strength (0-100) minus reused penalty */
    double avg_strength_ratio = (double)total_score_sum / (list->count * 5.0);
    double score = avg_strength_ratio * 100.0;

    /* Deduct points for reused and short passwords */
    double reused_ratio = (double)report->reused_count / list->count;
    double short_ratio = (double)report->short_count / list->count;

    score -= (reused_ratio * 30.0);
    score -= (short_ratio * 20.0);

    if (score < 0.0) score = 0.0;
    if (score > 100.0) score = 100.0;

    report->health_score = (int)(score + 0.5);
    return 1;
}
