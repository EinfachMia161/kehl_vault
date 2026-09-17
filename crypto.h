/**
 * @file crypto.h
 * @brief Self-contained cryptographic primitives according to RFC standards.
 *
 * Implements SHA-256 (RFC 6234), HMAC-SHA256 (RFC 2104), PBKDF2-HMAC-SHA256 (RFC 2898),
 * ChaCha20 stream cipher (RFC 8439), and constant-time memory comparison for timing-attack immunity.
 */

#ifndef KEHL_VAULT_CRYPTO_H
#define KEHL_VAULT_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CRYPTO_SHA256_HASH_SIZE 32
#define CRYPTO_SALT_SIZE 16
#define CRYPTO_NONCE_SIZE 12
#define CRYPTO_KEY_SIZE 32

/**
 * @brief SHA-256 state context structure.
 */
typedef struct {
    uint32_t state[8];   /**< 8 32-bit state words (A through H) */
    uint64_t count;      /**< Total number of bytes processed */
    uint8_t buffer[64];  /**< Internal 512-bit message block buffer */
} CryptoSha256Context;

/**
 * @brief Initializes a SHA-256 context.
 * @param ctx Pointer to CryptoSha256Context.
 */
void crypto_sha256_init(CryptoSha256Context* ctx);

/**
 * @brief Absorbs data bytes into the active SHA-256 context.
 * @param ctx Pointer to CryptoSha256Context.
 * @param data Input byte buffer.
 * @param len Length in bytes.
 */
void crypto_sha256_update(CryptoSha256Context* ctx, const uint8_t* data, size_t len);

/**
 * @brief Finalizes the SHA-256 hash calculation with padding and emits the 32-byte digest.
 * @param ctx Pointer to CryptoSha256Context.
 * @param hash Output 32-byte buffer.
 */
void crypto_sha256_final(CryptoSha256Context* ctx, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]);

/**
 * @brief One-shot utility to compute the SHA-256 digest of a message.
 * @param data Input data buffer.
 * @param len Data length in bytes.
 * @param hash Output 32-byte digest buffer.
 */
void crypto_sha256(const uint8_t* data, size_t len, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]);

/**
 * @brief Computes HMAC-SHA256 Message Authentication Code (RFC 2104).
 * @param key Secret authentication key.
 * @param key_len Key length in bytes.
 * @param data Input message data.
 * @param data_len Data length in bytes.
 * @param out Output 32-byte authentication tag.
 */
void crypto_hmac_sha256(
    const uint8_t* key,
    size_t key_len,
    const uint8_t* data,
    size_t data_len,
    uint8_t out[CRYPTO_SHA256_HASH_SIZE]
);

/**
 * @brief Derives cryptographic keys from a password using PBKDF2 with HMAC-SHA256 (RFC 2898).
 * @param password Master password string.
 * @param password_len Length of the password.
 * @param salt Random cryptographic salt.
 * @param salt_len Length of salt in bytes.
 * @param iterations Number of hashing rounds (e.g., 100,000).
 * @param out_key Output buffer for derived key material.
 * @param key_len Requested output key length in bytes.
 * @return 1 on success, 0 on failure.
 */
int crypto_pbkdf2_sha256(
    const char* password,
    size_t password_len,
    const uint8_t* salt,
    size_t salt_len,
    uint32_t iterations,
    uint8_t* out_key,
    size_t key_len
);

/**
 * @brief Encrypts or decrypts a buffer using the ChaCha20 stream cipher (RFC 8439).
 * @param key 256-bit (32-byte) secret key.
 * @param nonce 96-bit (12-byte) unique initialization nonce.
 * @param counter Initial 32-bit block counter (standard is 1).
 * @param input Input plaintext or ciphertext buffer.
 * @param output Output destination buffer.
 * @param length Number of bytes to transform.
 */
void crypto_chacha20_xor(
    const uint8_t key[CRYPTO_KEY_SIZE],
    const uint8_t nonce[CRYPTO_NONCE_SIZE],
    uint32_t counter,
    const uint8_t* input,
    uint8_t* output,
    size_t length
);

/**
 * @brief Constant-time byte comparison preventing side-channel timing attacks.
 * @param a First byte buffer.
 * @param b Second byte buffer.
 * @param length Buffer length to compare.
 * @return 1 if equal, 0 if different.
 */
int crypto_constant_time_equals(const uint8_t* a, const uint8_t* b, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_CRYPTO_H */
