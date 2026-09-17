/**
 * @file clipboard.c
 * @brief Implementation of cross-platform clipboard interactions.
 */

#include "clipboard.h"
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

int clipboard_copy_text(const char* text) {
    if (text == NULL) return 0;

    size_t len = strlen(text);
    if (!OpenClipboard(NULL)) {
        return 0;
    }

    EmptyClipboard();

    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, len + 1);
    if (hGlob == NULL) {
        CloseClipboard();
        return 0;
    }

    char* pBuf = (char*)GlobalLock(hGlob);
    if (pBuf == NULL) {
        GlobalFree(hGlob);
        CloseClipboard();
        return 0;
    }

    memcpy(pBuf, text, len + 1);
    GlobalUnlock(hGlob);

    if (SetClipboardData(CF_TEXT, hGlob) == NULL) {
        GlobalFree(hGlob);
        CloseClipboard();
        return 0;
    }

    CloseClipboard();
    return 1;
}

#else
#include <stdio.h>

int clipboard_copy_text(const char* text) {
    if (text == NULL) return 0;

    /* Try wl-copy (Wayland), xclip (X11), xsel, or pbcopy (macOS) */
    FILE* pipe = popen("wl-copy 2>/dev/null || xclip -selection clipboard 2>/dev/null || xsel --clipboard --input 2>/dev/null || pbcopy 2>/dev/null", "w");
    if (pipe == NULL) {
        return 0;
    }

    fputs(text, pipe);
    int status = pclose(pipe);
    return (status == 0) ? 1 : 0;
}

#endif
