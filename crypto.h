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

/* SHA-256 context */
typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t buffer[64];
} CryptoSha256Context;

void crypto_sha256_init(CryptoSha256Context* ctx);
void crypto_sha256_update(CryptoSha256Context* ctx, const uint8_t* data, size_t len);
void crypto_sha256_final(CryptoSha256Context* ctx, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]);
void crypto_sha256(const uint8_t* data, size_t len, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]);

/* HMAC-SHA256 */
void crypto_hmac_sha256(
    const uint8_t* key,
    size_t key_len,
    const uint8_t* data,
    size_t data_len,
    uint8_t out[CRYPTO_SHA256_HASH_SIZE]
);

/* PBKDF2 with HMAC-SHA256 */
int crypto_pbkdf2_sha256(
    const char* password,
    size_t password_len,
    const uint8_t* salt,
    size_t salt_len,
    uint32_t iterations,
    uint8_t* out_key,
    size_t key_len
);

/* ChaCha20 stream cipher */
void crypto_chacha20_xor(
    const uint8_t key[CRYPTO_KEY_SIZE],
    const uint8_t nonce[CRYPTO_NONCE_SIZE],
    uint32_t counter,
    const uint8_t* input,
    uint8_t* output,
    size_t length
);

/* Constant-time memory comparison to prevent timing attacks */
int crypto_constant_time_equals(const uint8_t* a, const uint8_t* b, size_t length);

#ifdef __cplusplus
}
#endif

#endif
