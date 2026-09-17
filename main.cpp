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

    std::cout << "Entries nach dem Hinzufuegen:\n";
    std::cout << "-----------------------------\n";
    entry_print_list(entries, entry_count);

    int update_successful =
            entry_update(
                    entries,
                    entry_count,
                    1,
                    "Geaendertes Schulkonto",
                    "neuer-benutzername@example.com",
                    "NeuesPasswort456!"
            );

    std::cout << "Bearbeiten: ";
    if (update_successful == 1) {
        std::cout << "erfolgreich.\n\n";
    } else {
        std::cout << "fehlgeschlagen.\n\n";
    }

    std::cout << "Entries nach dem Bearbeiten:\n";
    std::cout << "----------------------------\n";
    entry_print_list(entries, entry_count);

    int invalid_update =
            entry_update(
                    entries,
                    entry_count,
                    5,
                    "Ungueltiger Entry",
                    "ungueltig@example.com",
                    "WirdNichtGespeichert!"
            );

    std::cout << "Bearbeiten mit Index 5: ";
    if (invalid_update == 1) {
        std::cout << "erfolgreich.\n\n";
    } else {
        std::cout << "abgelehnt.\n\n";
    }

    int remove_successful =
            entry_remove(
                    entries,
                    &entry_count,
                    0
            );

    std::cout << "Loeschen von Entry 1: ";
    if (remove_successful == 1) {
        std::cout << "erfolgreich.\n\n";
    } else {
        std::cout << "fehlgeschlagen.\n\n";
    }

    std::cout << "Entries nach dem Loeschen:\n";
    std::cout << "--------------------------\n";
    entry_print_list(entries, entry_count);

    int invalid_remove =
            entry_remove(
                    entries,
                    &entry_count,
                    5
            );

    std::cout << "Loeschen mit Index 5: ";
    if (invalid_remove == 1) {
        std::cout << "erfolgreich.\n";
    } else {
        std::cout << "abgelehnt.\n";
    }

    return 0;
}