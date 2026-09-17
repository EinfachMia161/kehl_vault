/**
 * @file expiry.h
 * @brief Password expiration and credential age tracking for kehl-vault.
 *
 * Provides policy-based tracking of password ages, computing days until expiration
 * and identifying stale credentials requiring password rotation.
 */

#ifndef KEHL_VAULT_EXPIRY_H
#define KEHL_VAULT_EXPIRY_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXPIRY_POLICY_NONE 0
#define EXPIRY_POLICY_30_DAYS 30
#define EXPIRY_POLICY_60_DAYS 60
#define EXPIRY_POLICY_90_DAYS 90
#define EXPIRY_POLICY_180_DAYS 180
#define EXPIRY_POLICY_365_DAYS 365

#define EXPIRY_WARNING_THRESHOLD_DAYS 7

/**
 * @brief Expiry status states.
 */
typedef enum {
    EXPIRY_STATUS_OK = 0,
    EXPIRY_STATUS_WARNING = 1,  /* Expiring within threshold */
    EXPIRY_STATUS_EXPIRED = 2,  /* Past expiration date */
    EXPIRY_STATUS_UNKNOWN = 3
} ExpiryStatus;

/**
 * @brief Represents age and expiration metrics for a single entry.
 */
typedef struct {
    uint64_t last_modified;
    int policy_days;
    int age_days;
    int days_remaining;
    ExpiryStatus status;
} EntryExpiryInfo;

/**
 * @brief Calculates age in days from a given timestamp to the reference time.
 * @param last_modified Unix timestamp of the last modification.
 * @param reference_time Unix timestamp (or 0 for current system time).
 * @return Age in integer days.
 */
int expiry_calculate_age_days(uint64_t last_modified, uint64_t reference_time);

/**
 * @brief Evaluates the expiry status of an entry based on its age and assigned policy.
 * @param last_modified Unix timestamp of last password change.
 * @param policy_days Configured expiration duration in days (0 for no policy).
 * @param reference_time Current reference timestamp (0 for now).
 * @param out_info Pointer to receive evaluated ExpiryInfo structure.
 * @return 1 on successful evaluation, 0 on invalid parameters.
 */
int expiry_evaluate(
    uint64_t last_modified,
    int policy_days,
    uint64_t reference_time,
    EntryExpiryInfo* out_info
);

/**
 * @brief Returns a human-readable string representation of the expiry status.
 * @param status ExpiryStatus enum value.
 * @return Constant descriptive string.
 */
const char* expiry_status_to_string(ExpiryStatus status);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_EXPIRY_H */
