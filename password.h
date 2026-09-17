#ifndef KEHL_VAULT_PASSWORD_H
#define KEHL_VAULT_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif

int password_is_long_enough(const char* password, int minimum_length);

#ifdef __cplusplus
}
#endif

#endif
