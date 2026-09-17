/**
 * @file totp.h
 * @brief Time-Based One-Time Password (TOTP) generator conforming to RFC 6238 and RFC 4226.
 *
 * Provides Base32 secret decoding, HMAC-SHA1 token derivation, and 30-second rolling
 * 6-digit code generation for Two-Factor Authentication (2FA).
 */

#ifndef KEHL_VAULT_TOTP_H
#define KEHL_VAULT_TOTP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TOTP_DEFAULT_DIGITS 6
#define TOTP_DEFAULT_PERIOD 30

/**
 * @brief Decodes an RFC 4648 Base32 encoded secret key into raw binary bytes.
 * @param base32_str Input Base32 string (case-insensitive, ignores spaces and hyphens).
 * @param out_buf Output buffer for binary bytes.
 * @param out_size Capacity of output buffer.
 * @return Number of decoded bytes on success, or -1 on invalid format/insufficient buffer.
 */
int totp_base32_decode(const char* base32_str, uint8_t* out_buf, size_t out_size);

/**
 * @brief Generates a TOTP code for an explicit Unix timestamp.
 * @param base32_secret Base32 secret key string.
 * @param timestamp Unix epoch timestamp in seconds.
 * @param digits Number of code digits (typically 6 or 8).
 * @param period Time step in seconds (standard is 30).
 * @param output Buffer to receive the formatted numeric code (null-terminated).
 * @param output_size Capacity of the output buffer (must be at least digits + 1).
 * @return 1 on success, 0 on failure.
 */
int totp_generate(
    const char* base32_secret,
    uint64_t timestamp,
    int digits,
    int period,
    char* output,
    size_t output_size
);

/**
 * @brief Generates a TOTP code for the current system time.
 * @param base32_secret Base32 secret key string.
 * @param output Buffer to receive the 6-digit code string.
 * @param output_size Size of the output buffer.
 * @param remaining_seconds Optional pointer to receive remaining seconds in current interval (0-29).
 * @return 1 on success, 0 on failure.
 */
int totp_generate_current(
    const char* base32_secret,
    char* output,
    size_t output_size,
    int* remaining_seconds
);

/**
 * @brief Verifies a user-provided code against the current time with an allowable clock drift window.
 * @param base32_secret Base32 secret key string.
 * @param code User-provided token code string.
 * @param timestamp Unix timestamp (or 0 for current time).
 * @param window Number of time periods (+/-) to accept (typically 1).
 * @return 1 if code matches within window, 0 otherwise.
 */
int totp_verify(
    const char* base32_secret,
    const char* code,
    uint64_t timestamp,
    int window
);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_TOTP_H */
