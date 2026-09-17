#ifndef KEHL_VAULT_CLIPBOARD_H
#define KEHL_VAULT_CLIPBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Copies text to system clipboard. Returns 1 on success, 0 on failure. */
int clipboard_copy_text(const char* text);

#ifdef __cplusplus
}
#endif

#endif
