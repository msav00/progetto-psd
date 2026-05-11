#include "menu.h"
#include "stdio.h"
#include "stdlib.h"

//Number of menu entries besides the exit option
#define MAX_MENU_ENTRIES 5

char* entries[MAX_MENU_ENTRIES] = {
  "Issue new report",
  "Update report",
  "List reports",
  "Search report",
  "Generate overview"
};

//todo: remove placeholder and set callbacks when implemented
void placeholder(char *str) { printf("%s\n", str); }

menu_callback callbacks[MAX_MENU_ENTRIES] = {
    placeholder,
    placeholder,
    placeholder,
    placeholder,
    placeholder
};

void doMenu() {
    int opt;
    do {
        printf("Available options:\n");
        for (int i = 0; i < MAX_MENU_ENTRIES; i++) {
            printf("\t(%d): %s\n", i+1, entries[i]);
        }

        printf("\t(0): Exit\nSelect an option (0-%d): ", MAX_MENU_ENTRIES);   
        scanf("%d", &opt);
        system("clear");

        if (opt >= 0 && opt <= MAX_MENU_ENTRIES) {
            if (opt == 0) break;
            callbacks[opt-1]("Callback Test");

        } else printf("Invalid option %d\n", opt);

        /* //Old implementation w/o callbacks
        switch (opt)
        {
            case 0: break;
            case 1:
                break;
            
            default:
                printf("Invalid option %d\n", opt);
                break;
        }
        */

    } while (opt != 0);
}