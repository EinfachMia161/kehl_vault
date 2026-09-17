/**
 * @file crypto.c
 * @brief Implementation of RFC-standard cryptographic hash and cipher primitives.
 */

#include "crypto.h"
#include <string.h>
#include <stdlib.h>

/* --- SHA-256 Implementation (RFC 6234) --- */

#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTR32(x, 2) ^ ROTR32(x, 13) ^ ROTR32(x, 22))
#define EP1(x) (ROTR32(x, 6) ^ ROTR32(x, 11) ^ ROTR32(x, 25))
#define SIG0(x) (ROTR32(x, 7) ^ ROTR32(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTR32(x, 17) ^ ROTR32(x, 19) ^ ((x) >> 10))

static const uint32_t K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void sha256_transform(CryptoSha256Context* ctx, const uint8_t data[64]) {
    uint32_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    for (i = 0, j = 0; i < 16; ++i, j += 4) {
        m[i] = ((uint32_t)data[j] << 24) |
               ((uint32_t)data[j + 1] << 16) |
               ((uint32_t)data[j + 2] << 8) |
               ((uint32_t)data[j + 3]);
    }
    for (; i < 64; ++i) {
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + K256[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void crypto_sha256_init(CryptoSha256Context* ctx) {
    if (!ctx) return;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->count = 0;
}

void crypto_sha256_update(CryptoSha256Context* ctx, const uint8_t* data, size_t len) {
    if (!ctx || !data || len == 0) return;
    size_t i;
    for (i = 0; i < len; ++i) {
        ctx->buffer[ctx->count % 64] = data[i];
        ctx->count++;
        if ((ctx->count % 64) == 0) {
            sha256_transform(ctx, ctx->buffer);
        }
    }
}

void crypto_sha256_final(CryptoSha256Context* ctx, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]) {
    if (!ctx || !hash) return;
    uint32_t i = (uint32_t)(ctx->count % 64);

    ctx->buffer[i++] = 0x80;
    if (i > 56) {
        while (i < 64) {
            ctx->buffer[i++] = 0x00;
        }
        sha256_transform(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    } else {
        while (i < 56) {
            ctx->buffer[i++] = 0x00;
        }
    }

    uint64_t total_bits = ctx->count * 8;
    for (int b = 7; b >= 0; --b) {
        ctx->buffer[56 + (7 - b)] = (uint8_t)((total_bits >> (b * 8)) & 0xFF);
    }
    sha256_transform(ctx, ctx->buffer);

    for (i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {
            hash[j * 4 + i] = (uint8_t)((ctx->state[j] >> (24 - i * 8)) & 0x000000FF);
        }
    }
}

void crypto_sha256(const uint8_t* data, size_t len, uint8_t hash[CRYPTO_SHA256_HASH_SIZE]) {
    CryptoSha256Context ctx;
    crypto_sha256_init(&ctx);
    crypto_sha256_update(&ctx, data, len);
    crypto_sha256_final(&ctx, hash);
}

/* --- HMAC-SHA256 Implementation (RFC 2104) --- */

void crypto_hmac_sha256(
    const uint8_t* key,
    size_t key_len,
    const uint8_t* data,
    size_t data_len,
    uint8_t out[CRYPTO_SHA256_HASH_SIZE]
) {
    uint8_t k_pad[64];
    uint8_t tk[CRYPTO_SHA256_HASH_SIZE];

    if (key_len > 64) {
        crypto_sha256(key, key_len, tk);
        key = tk;
        key_len = CRYPTO_SHA256_HASH_SIZE;
    }

    memset(k_pad, 0x36, sizeof(k_pad));
    for (size_t i = 0; i < key_len; ++i) {
        k_pad[i] ^= key[i];
    }

    CryptoSha256Context ctx;
    crypto_sha256_init(&ctx);
    crypto_sha256_update(&ctx, k_pad, 64);
    crypto_sha256_update(&ctx, data, data_len);
    crypto_sha256_final(&ctx, out);

    memset(k_pad, 0x5c, sizeof(k_pad));
    for (size_t i = 0; i < key_len; ++i) {
        k_pad[i] ^= key[i];
    }

    crypto_sha256_init(&ctx);
    crypto_sha256_update(&ctx, k_pad, 64);
    crypto_sha256_update(&ctx, out, CRYPTO_SHA256_HASH_SIZE);
    crypto_sha256_final(&ctx, out);
}

/* --- PBKDF2-HMAC-SHA256 (RFC 2898) --- */

int crypto_pbkdf2_sha256(
    const char* password,
    size_t password_len,
    const uint8_t* salt,
    size_t salt_len,
    uint32_t iterations,
    uint8_t* out_key,
    size_t key_len
) {
    if (!password || !salt || salt_len == 0 || iterations == 0 || !out_key || key_len == 0) {
        return 0;
    }

    uint32_t block_count = (uint32_t)((key_len + CRYPTO_SHA256_HASH_SIZE - 1) / CRYPTO_SHA256_HASH_SIZE);
    uint8_t u[CRYPTO_SHA256_HASH_SIZE];
    uint8_t t[CRYPTO_SHA256_HASH_SIZE];

    /* Allocate buffer for salt + 4-byte big-endian block index */
    uint8_t* s_block = (uint8_t*)malloc(salt_len + 4);
    if (!s_block) return 0;
    memcpy(s_block, salt, salt_len);

    for (uint32_t i = 1; i <= block_count; ++i) {
        s_block[salt_len]     = (uint8_t)((i >> 24) & 0xFF);
        s_block[salt_len + 1] = (uint8_t)((i >> 16) & 0xFF);
        s_block[salt_len + 2] = (uint8_t)((i >> 8) & 0xFF);
        s_block[salt_len + 3] = (uint8_t)(i & 0xFF);

        crypto_hmac_sha256((const uint8_t*)password, password_len, s_block, salt_len + 4, u);
        memcpy(t, u, CRYPTO_SHA256_HASH_SIZE);

        for (uint32_t j = 1; j < iterations; ++j) {
            crypto_hmac_sha256((const uint8_t*)password, password_len, u, CRYPTO_SHA256_HASH_SIZE, u);
            for (size_t k = 0; k < CRYPTO_SHA256_HASH_SIZE; ++k) {
                t[k] ^= u[k];
            }
        }

        size_t offset = (i - 1) * CRYPTO_SHA256_HASH_SIZE;
        size_t bytes_to_copy = key_len - offset;
        if (bytes_to_copy > CRYPTO_SHA256_HASH_SIZE) {
            bytes_to_copy = CRYPTO_SHA256_HASH_SIZE;
        }
        memcpy(out_key + offset, t, bytes_to_copy);
    }

    free(s_block);
    return 1;
}

/* --- ChaCha20 Implementation (RFC 8439) --- */

#define CHACHA_ROTL32(v, n) (((v) << (n)) | ((v) >> (32 - (n))))
#define CHACHA_QUARTERROUND(a, b, c, d) \
    a += b; d ^= a; d = CHACHA_ROTL32(d, 16); \
    c += d; b ^= c; b = CHACHA_ROTL32(d, 12); \
    a += b; d ^= a; d = CHACHA_ROTL32(d, 8);  \
    c += d; b ^= c; b = CHACHA_ROTL32(d, 7);

static uint32_t load_le32(const uint8_t* p) {
    return ((uint32_t)p[0]) |
           (((uint32_t)p[1]) << 8) |
           (((uint32_t)p[2]) << 16) |
           (((uint32_t)p[3]) << 24);
}

static void store_le32(uint8_t* p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}

static void chacha20_block(const uint32_t in_state[16], uint8_t key_stream[64]) {
    uint32_t s[16];
    memcpy(s, in_state, sizeof(s));

    for (int i = 0; i < 10; ++i) {
        /* Column rounds */
        CHACHA_QUARTERROUND(s[0], s[4], s[8],  s[12]);
        CHACHA_QUARTERROUND(s[1], s[5], s[9],  s[13]);
        CHACHA_QUARTERROUND(s[2], s[6], s[10], s[14]);
        CHACHA_QUARTERROUND(s[3], s[7], s[11], s[15]);

        /* Diagonal rounds */
        CHACHA_QUARTERROUND(s[0], s[5], s[10], s[15]);
        CHACHA_QUARTERROUND(s[1], s[6], s[11], s[12]);
        CHACHA_QUARTERROUND(s[2], s[7], s[8],  s[13]);
        CHACHA_QUARTERROUND(s[3], s[4], s[9],  s[14]);
    }

    for (int i = 0; i < 16; ++i) {
        store_le32(key_stream + i * 4, s[i] + in_state[i]);
    }
}

void crypto_chacha20_xor(
    const uint8_t key[CRYPTO_KEY_SIZE],
    const uint8_t nonce[CRYPTO_NONCE_SIZE],
    uint32_t counter,
    const uint8_t* input,
    uint8_t* output,
    size_t length
) {
    if (!key || !nonce || !input || !output || length == 0) return;

    uint32_t state[16];
    /* Constants "expand 32-byte k" */
    state[0] = 0x61707865;
    state[1] = 0x3320646e;
    state[2] = 0x79622d32;
    state[3] = 0x6b206574;

    /* Key (256-bit) */
    for (int i = 0; i < 8; ++i) {
        state[4 + i] = load_le32(key + i * 4);
    }

    /* Counter */
    state[12] = counter;

    /* Nonce (96-bit) */
    state[13] = load_le32(nonce + 0);
    state[14] = load_le32(nonce + 4);
    state[15] = load_le32(nonce + 8);

    uint8_t keystream[64];
    size_t offset = 0;

    while (offset < length) {
        state[12] = counter++;
        chacha20_block(state, keystream);

        size_t block_len = length - offset;
        if (block_len > 64) block_len = 64;

        for (size_t i = 0; i < block_len; ++i) {
            output[offset + i] = input[offset + i] ^ keystream[i];
        }

        offset += block_len;
    }
}

int crypto_constant_time_equals(const uint8_t* a, const uint8_t* b, size_t length) {
    if (!a || !b) return 0;
    uint8_t diff = 0;
    for (size_t i = 0; i < length; ++i) {
        diff |= (a[i] ^ b[i]);
    }
    return diff == 0 ? 1 : 0;
}
