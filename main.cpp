#include "entry.h"

#include <iostream>

int main() {
    const int maximum_entries = 3;

    Entry entries[maximum_entries];
    int entry_count = 0;

    entry_add(
            entries,
            maximum_entries,
            &entry_count,
            "Beispielkonto",
            "mimi@example.com",
            "TestPasswort123!"
    );

    entry_add(
            entries,
            maximum_entries,
            &entry_count,
            "Schulkonto",
            "mimi@schule.example",
            "SchulPasswort456!"
    );

    entry_add(
            entries,
            maximum_entries,
            &entry_count,
            "Testkonto",
            "test@example.com",
            "NochEinTest789!"
    );

    std::cout << "Entries vor der Bearbeitung:\n";
    std::cout << "----------------------------\n";
    entry_print_list(entries, entry_count);

    Entry* selected_entry =
            entry_get(entries, entry_count, 1);

    if (selected_entry != nullptr) {
        std::cout << "Entry mit Index 1 wurde gefunden.\n";

        entry_create(
                selected_entry,
                "Geaendertes Schulkonto",
                "neuer-benutzername@example.com",
                "NeuesPasswort456!"
        );
    } else {
        std::cout << "Entry mit Index 1 wurde nicht gefunden.\n";
    }

    std::cout << "\n";
    std::cout << "Entries nach der Bearbeitung:\n";
    std::cout << "-----------------------------\n";
    entry_print_list(entries, entry_count);

    Entry* invalid_entry =
            entry_get(entries, entry_count, 5);

    if (invalid_entry == nullptr) {
        std::cout << "Index 5 wurde korrekt abgelehnt.\n";
    }

    int remove_successful =
            entry_remove(
                    entries,
                    &entry_count,
                    0
            );

    std::cout << "\nEntry mit Index 0 loeschen: ";

    if (remove_successful == 1) {
        std::cout << "erfolgreich.\n";
    } else {
        std::cout << "fehlgeschlagen.\n";
    }

    std::cout << "\n";
    std::cout << "Entries nach dem Loeschen:\n";
    std::cout << "--------------------------\n";
    entry_print_list(entries, entry_count);

    return 0;
}