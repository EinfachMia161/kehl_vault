#include "entry.h"

#include <iostream>

int main() {
    EntryList list;

    int initialization_successful =
            entry_list_init(&list, 2);

    if (initialization_successful == 0) {
        std::cout << "Entry-Liste konnte nicht initialisiert werden.\n";
        return 1;
    }

    std::cout << "Startzustand:\n";
    std::cout << "-------------\n";
    entry_list_print(&list);

    entry_list_add(
            &list,
            "Beispielkonto",
            "mimi@example.com",
            "TestPasswort123!"
    );

    entry_list_add(
            &list,
            "Schulkonto",
            "mimi@schule.example",
            "SchulPasswort456!"
    );

    std::cout << "Nach zwei Entries:\n";
    std::cout << "------------------\n";
    entry_list_print(&list);

    std::cout << "Kapazitaet vor dem dritten Entry: "
              << list.capacity
              << "\n\n";

    entry_list_add(
            &list,
            "Testkonto",
            "test@example.com",
            "NochEinTest789!"
    );

    std::cout << "Nach dem dritten Entry:\n";
    std::cout << "----------------------\n";
    entry_list_print(&list);

    std::cout << "Kapazitaet nach dem automatischen Wachsen: "
              << list.capacity
              << "\n\n";

    Entry* selected_entry =
            entry_list_get(&list, 1);

    if (selected_entry != nullptr) {
        entry_create(
                selected_entry,
                "Geaendertes Schulkonto",
                "neuer-benutzername@example.com",
                "NeuesPasswort456!"
        );

        std::cout << "Entry mit Index 1 wurde bearbeitet.\n\n";
    }

    entry_list_remove(&list, 0);

    std::cout << "Nach dem Loeschen von Entry 1:\n";
    std::cout << "------------------------------\n";
    entry_list_print(&list);

    Entry* invalid_entry =
            entry_list_get(&list, 99);

    if (invalid_entry == nullptr) {
        std::cout << "Ungueltiger Index wurde korrekt abgelehnt.\n";
    }

    entry_list_destroy(&list);

    std::cout << "Entry-Liste wurde freigegeben.\n";

    return 0;
}