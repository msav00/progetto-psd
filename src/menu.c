#include "menu.h"
#include "stdio.h"
#include "stdlib.h"

#include "menu_impl.h"

//Number of menu entries besides the exit option
#define MAX_MENU_ENTRIES 4

char* entries[MAX_MENU_ENTRIES] = {
  "Issue new report",
  "List all reports",
  "Search report",
  "Generate overview"
};

menu_callback callbacks[MAX_MENU_ENTRIES] = {
    menuNewReport,
    menuListReports,
    menuFilterReports,
    menuOverview
};

/// @brief Executes looping menu logic with the specified entries and their associated callbacks in the implementation.
void doMenu(hashtable h) {
    int opt;
    do {
        printf("Available options:\n");
        for (int i = 0; i < MAX_MENU_ENTRIES; i++) {
            printf("\t(%d): %s\n", i+1, entries[i]);
        }

        printf("\t(0): Exit\nSelect an option (0-%d): ", MAX_MENU_ENTRIES);   
        scanf("%d", &opt);
        getchar(); //consume newline for upcoming string input functions.
        system("clear");

        if (opt >= 0 && opt <= MAX_MENU_ENTRIES) {
            if (opt == 0) break;
            callbacks[opt-1](h);

        } else printf("Invalid option %d\n", opt);

    } while (opt != 0);
}