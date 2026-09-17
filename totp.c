/**
 * @file totp.c
 * @brief Implementation of RFC 6238 Time-Based One-Time Password algorithm.
 */

#include "totp.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>

/* --- Internal SHA-1 & HMAC-SHA1 Engine (RFC 3174) --- */

typedef struct {
    uint32_t state[5];
    uint64_t count;
    uint8_t buffer[64];
} Sha1Context;

#define SHA1_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

static void sha1_transform(Sha1Context* ctx, const uint8_t buffer[64]) {
    uint32_t a = ctx->state[0];
    uint32_t b = ctx->state[1];
    uint32_t c = ctx->state[2];
    uint32_t d = ctx->state[3];
    uint32_t e = ctx->state[4];
    uint32_t w[80];

    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)buffer[i * 4] << 24) |
               ((uint32_t)buffer[i * 4 + 1] << 16) |
               ((uint32_t)buffer[i * 4 + 2] << 8) |
               ((uint32_t)buffer[i * 4 + 3]);
    }

    for (int i = 16; i < 80; i++) {
        w[i] = SHA1_ROTL(1, w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);
    }

    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        } else if (i < 40) {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        } else if (i < 60) {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        } else {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        uint32_t temp = SHA1_ROTL(5, a) + f + e + k + w[i];
        e = d;
        d = c;
        c = SHA1_ROTL(30, b);
        b = a;
        a = temp;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
}

static void sha1_init(Sha1Context* ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count = 0;
}

static void sha1_update(Sha1Context* ctx, const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ctx->buffer[ctx->count % 64] = data[i];
        ctx->count++;
        if ((ctx->count % 64) == 0) {
            sha1_transform(ctx, ctx->buffer);
        }
    }
}

static void sha1_final(Sha1Context* ctx, uint8_t hash[20]) {
    uint32_t i = (uint32_t)(ctx->count % 64);
    ctx->buffer[i++] = 0x80;
    if (i > 56) {
        while (i < 64) ctx->buffer[i++] = 0x00;
        sha1_transform(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    } else {
        while (i < 56) ctx->buffer[i++] = 0x00;
    }

    uint64_t total_bits = ctx->count * 8;
    for (int b = 7; b >= 0; b--) {
        ctx->buffer[56 + (7 - b)] = (uint8_t)((total_bits >> (b * 8)) & 0xFF);
    }
    sha1_transform(ctx, ctx->buffer);

    for (i = 0; i < 5; i++) {
        hash[i * 4]     = (uint8_t)((ctx->state[i] >> 24) & 0xFF);
        hash[i * 4 + 1] = (uint8_t)((ctx->state[i] >> 16) & 0xFF);
        hash[i * 4 + 2] = (uint8_t)((ctx->state[i] >> 8) & 0xFF);
        hash[i * 4 + 3] = (uint8_t)(ctx->state[i] & 0xFF);
    }
}

static void hmac_sha1(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t out[20]) {
    uint8_t k_pad[64];
    uint8_t tk[20];

    if (key_len > 64) {
        Sha1Context tctx;
        sha1_init(&tctx);
        sha1_update(&tctx, key, key_len);
        sha1_final(&tctx, tk);
        key = tk;
        key_len = 20;
    }

    memset(k_pad, 0x36, sizeof(k_pad));
    for (size_t i = 0; i < key_len; i++) {
        k_pad[i] ^= key[i];
    }

    Sha1Context ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, k_pad, 64);
    sha1_update(&ctx, data, data_len);
    sha1_final(&ctx, out);

    memset(k_pad, 0x5c, sizeof(k_pad));
    for (size_t i = 0; i < key_len; i++) {
        k_pad[i] ^= key[i];
    }

    sha1_init(&ctx);
    sha1_update(&ctx, k_pad, 64);
    sha1_update(&ctx, out, 20);
    sha1_final(&ctx, out);
}

/* --- Base32 Decoding (RFC 4648) --- */

int totp_base32_decode(const char* base32_str, uint8_t* out_buf, size_t out_size) {
    if (!base32_str || !out_buf || out_size == 0) return -1;

    int buffer = 0;
    int bits_left = 0;
    size_t count = 0;

    for (size_t i = 0; base32_str[i] != '\0'; i++) {
        char ch = base32_str[i];
        if (ch == ' ' || ch == '-' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '=') {
            continue;
        }

        int val = -1;
        ch = (char)toupper((unsigned char)ch);
        if (ch >= 'A' && ch <= 'Z') {
            val = ch - 'A';
        } else if (ch >= '2' && ch <= '7') {
            val = ch - '2' + 26;
        } else {
            return -1; /* Invalid base32 character */
        }

        buffer = (buffer << 5) | val;
        bits_left += 5;

        if (bits_left >= 8) {
            if (count >= out_size) return -1;
            out_buf[count++] = (uint8_t)((buffer >> (bits_left - 8)) & 0xFF);
            bits_left -= 8;
        }
    }

    return (int)count;
}

/* --- TOTP Code Generation --- */

int totp_generate(
    const char* base32_secret,
    uint64_t timestamp,
    int digits,
    int period,
    char* output,
    size_t output_size
) {
    if (!base32_secret || !output || digits <= 0 || digits > 8 || period <= 0) {
        return 0;
    }
    if (output_size < (size_t)(digits + 1)) {
        return 0;
    }

    uint8_t secret_bytes[128];
    int secret_len = totp_base32_decode(base32_secret, secret_bytes, sizeof(secret_bytes));
    if (secret_len <= 0) {
        return 0;
    }

    uint64_t steps = timestamp / (uint64_t)period;
    uint8_t challenge[8];
    for (int i = 7; i >= 0; i--) {
        challenge[i] = (uint8_t)(steps & 0xFF);
        steps >>= 8;
    }

    uint8_t hash[20];
    hmac_sha1(secret_bytes, (size_t)secret_len, challenge, 8, hash);

    /* Dynamic truncation (RFC 4226) */
    int offset = hash[19] & 0x0F;
    uint32_t binary =
        ((hash[offset] & 0x7F) << 24) |
        ((hash[offset + 1] & 0xFF) << 16) |
        ((hash[offset + 2] & 0xFF) << 8) |
        (hash[offset + 3] & 0xFF);

    uint32_t mod = 1;
    for (int i = 0; i < digits; i++) {
        mod *= 10;
    }

    uint32_t token = binary % mod;

    char format[16];
    snprintf(format, sizeof(format), "%%0%dlu", digits);
    snprintf(output, output_size, format, (unsigned long)token);

    return 1;
}

int totp_generate_current(
    const char* base32_secret,
    char* output,
    size_t output_size,
    int* remaining_seconds
) {
    time_t now = time(NULL);
    if (remaining_seconds != NULL) {
        *remaining_seconds = (int)(TOTP_DEFAULT_PERIOD - (now % TOTP_DEFAULT_PERIOD));
    }
    return totp_generate(base32_secret, (uint64_t)now, TOTP_DEFAULT_DIGITS, TOTP_DEFAULT_PERIOD, output, output_size);
}

int totp_verify(
    const char* base32_secret,
    const char* code,
    uint64_t timestamp,
    int window
) {
    if (!base32_secret || !code) return 0;

    if (timestamp == 0) {
        timestamp = (uint64_t)time(NULL);
    }

    char generated[16];
    for (int i = -window; i <= window; i++) {
        uint64_t check_time = timestamp + (int64_t)(i * TOTP_DEFAULT_PERIOD);
        if (totp_generate(base32_secret, check_time, TOTP_DEFAULT_DIGITS, TOTP_DEFAULT_PERIOD, generated, sizeof(generated))) {
            if (strcmp(generated, code) == 0) {
                return 1;
            }
        }
    }

    return 0;
}
