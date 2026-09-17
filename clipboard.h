/**
 * @file clipboard.h
 * @brief Secure clipboard integration for Windows and POSIX environments.
 *
 * Provides cross-platform capability to copy sensitive passwords and tokens
 * directly to the system clipboard to prevent console history leaks.
 */

#ifndef KEHL_VAULT_CLIPBOARD_H
#define KEHL_VAULT_CLIPBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Copies text to the system clipboard.
 * @param text Null-terminated text string to place on clipboard.
 * @return 1 on success, 0 on failure.
 */
int clipboard_copy_text(const char* text);

#ifdef __cplusplus
}
#endif

#endif /* KEHL_VAULT_CLIPBOARD_H */
