#ifndef KEHL_VAULT_IMPEX_H
#define KEHL_VAULT_IMPEX_H

#include "entry.h"

#ifdef __cplusplus
extern "C" {
#endif

int vault_export_csv(const EntryList* list, const char* filepath);
int vault_import_csv(EntryList* list, const char* filepath, int* imported_count);

int vault_export_json(const EntryList* list, const char* filepath);
int vault_import_json(EntryList* list, const char* filepath, int* imported_count);

#ifdef __cplusplus
}
#endif

#endif
