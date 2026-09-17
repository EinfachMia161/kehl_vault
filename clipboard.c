#include "clipboard.h"
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

int clipboard_copy_text(const char* text) {
    if (text == NULL) {
        return 0;
    }

    size_t len = strlen(text);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len + 1);
    if (hMem == NULL) {
        return 0;
    }

    char* pMem = (char*)GlobalLock(hMem);
    if (pMem == NULL) {
        GlobalFree(hMem);
        return 0;
    }

    memcpy(pMem, text, len + 1);
    GlobalUnlock(hMem);

    if (!OpenClipboard(NULL)) {
        GlobalFree(hMem);
        return 0;
    }

    EmptyClipboard();
    if (SetClipboardData(CF_TEXT, hMem) == NULL) {
        CloseClipboard();
        GlobalFree(hMem);
        return 0;
    }

    CloseClipboard();
    return 1;
}

#else
#include <stdio.h>

int clipboard_copy_text(const char* text) {
    if (text == NULL) {
        return 0;
    }

    /* Try wl-copy (Wayland), xclip, xsel, pbcopy (macOS) */
    FILE* pipe = popen("wl-copy 2>/dev/null || xclip -selection clipboard 2>/dev/null || xsel -b 2>/dev/null || pbcopy 2>/dev/null", "w");
    if (pipe == NULL) {
        return 0;
    }

    fputs(text, pipe);
    int status = pclose(pipe);
    return (status == 0) ? 1 : 0;
}

#endif
