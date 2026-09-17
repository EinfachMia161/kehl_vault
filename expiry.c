/**
 * @file expiry.c
 * @brief Implementation of credential age calculation and policy checking.
 */

#include "expiry.h"
#include <string.h>

#define SECONDS_PER_DAY (24 * 60 * 60)

int expiry_calculate_age_days(uint64_t last_modified, uint64_t reference_time) {
    if (reference_time == 0) {
        reference_time = (uint64_t)time(NULL);
    }

    if (reference_time < last_modified) {
        return 0;
    }

    uint64_t diff_seconds = reference_time - last_modified;
    return (int)(diff_seconds / SECONDS_PER_DAY);
}

int expiry_evaluate(
    uint64_t last_modified,
    int policy_days,
    uint64_t reference_time,
    EntryExpiryInfo* out_info
) {
    if (!out_info) {
        return 0;
    }

    if (reference_time == 0) {
        reference_time = (uint64_t)time(NULL);
    }

    out_info->last_modified = last_modified;
    out_info->policy_days = policy_days;
    out_info->age_days = expiry_calculate_age_days(last_modified, reference_time);

    if (policy_days <= 0) {
        out_info->days_remaining = 999999;
        out_info->status = EXPIRY_STATUS_OK;
        return 1;
    }

    out_info->days_remaining = policy_days - out_info->age_days;

    if (out_info->days_remaining < 0) {
        out_info->status = EXPIRY_STATUS_EXPIRED;
    } else if (out_info->days_remaining <= EXPIRY_WARNING_THRESHOLD_DAYS) {
        out_info->status = EXPIRY_STATUS_WARNING;
    } else {
        out_info->status = EXPIRY_STATUS_OK;
    }

    return 1;
}

const char* expiry_status_to_string(ExpiryStatus status) {
    switch (status) {
        case EXPIRY_STATUS_OK:
            return "Active / Good";
        case EXPIRY_STATUS_WARNING:
            return "Expiring Soon";
        case EXPIRY_STATUS_EXPIRED:
            return "Expired (Needs Rotation)";
        default:
            return "Unknown";
    }
}
