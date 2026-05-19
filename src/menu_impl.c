#include "menu_impl.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define NAME_BUFSIZE 256
#define DESC_BUFSIZE 256

#pragma region Internal Utility Functions

/// @brief Safely reads a string from stdin in a limited buffer and assigns it to the given pointer.
/// @param dest Pointer to assign the read string to.
void string_input(char * dest) {
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        int len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        strncpy(dest, buffer, len);
    }
}

/// @brief Prints the given list of reports. Used in many menu functions to list multiple reports.
/// @param root Pointer to the root of the list of reports.
void printList(item *root) {
    item *curr_node;
    int count = 0;
    curr_node = root;
    while (curr_node) {
        printf("\t(%d): %s\n", (count++)+1, curr_node->value->id);
        curr_node = curr_node->next;
    }
}

/// @brief Walks the given list until the item at the given index is reached.
/// @param root Pointer to the root of the list of reports.
/// @param index Position of the list node to return.
/// @return Pointer to the node at the given position or NULL if not found.
item *getItem(item *root, int index) {
    item *curr = root;
    int count = 1;
    while (curr) {
        if (count == index) return curr;
        count++;
        curr = curr->next;
    }
    return NULL;
}

/// @brief Frees the given list from memory, starting from the root. Used in most menu functions after exiting search/listing funtions.
/// @param root Pointer to the list to delete.
void freeList(item *root) {
    item *cur = root;
    while (cur) {
        item* next = cur->next;
        free(cur);
        cur = next;
    }
}

/// @brief Presents options for the given report. Used in menu functions to modify selected reports.
/// @param report Pointer to the report to modify/view.
void submenuReportDetails(hashtable h, Report *report) {
    int opt = 0;
    do
    {
        printReport(report);
        printf("Available operations:\n\t(1): Update state\n\t(2): Set priority\n\t(3): Delete report\n\t(0): Back\nSelect operation (0-3): ");
        scanf("%d", &opt);
        getchar();
        system("clear");
        
        switch (opt)
        {
            case 1:
                int state = Open;
                printf("Possible values:\n\t1 - Open\n\t2 - Ongoing\n\t3 - Closed\n[1-3 or 0 to go back]> ");
                scanf("%d", &state);
                if (state > 0 && state < 4) setReportState(report, (IssueState)(state-1));
                break;

            case 2:
                int urgency = Normal;
                printf("Possible values:\n\t1 - Low\n\t2 - Below Normal\n\t3 - Normal (Default)\n\t4 - Above Normal\n\t5 - High Priority\n[1-5 or 0 to go back]> ");
                scanf("%d", &urgency);
                if (urgency > 0 && state < 6) setReportUrgency(report, (IssueUrgency)(urgency-3));
                break;

            case 3:
                char confirm = 'n';
                printf("Are you sure you want to delete this report? (y/n): ");
                confirm = getchar();
                if (confirm == 'Y' || confirm == 'y') {
                    deleteReport(deleteHashtableItem(h, report->id));
                    return;
                }
                break;

            case 0: break;
        }
    } while (opt != 0);
    
}

#pragma endregion

void menuNewReport(hashtable h) {
    //Input strings first
    char *name = malloc(sizeof(char) * NAME_BUFSIZE), *desc = malloc(sizeof(char) * DESC_BUFSIZE);
    printf("Insert Issuer's Name: ");
    string_input(name);
    printf("Insert Description: ");
    string_input(desc);
    
    //Input and format date and time
    struct tm* date = malloc(sizeof(struct tm));
    printf("Insert date and time (HH:MM DD/MM/YYYY): ");
    scanf("%d:%d %d/%d/%d", &date->tm_hour, &date->tm_min, &date->tm_mday, &date->tm_mon, &date->tm_year);
    
    //Apply time offsets (months are stored 0-11, year as offset from year 1900 and days 0-6)
    date->tm_mon -= 1;
    date->tm_year -= 1900;
    date->tm_wday -=1;

    //Input a valid category
    printf("Choose a category:\n\t(1): Road\n\t(2): Illumination\n\t(3): Garbage Disposal\n\t(4): Public Services\n[1-4]> ");
    int cat;
    scanf("%d", &cat);
    if (cat < 1 || cat > 4) {
        printf("Invalid category, aborting...\n");
        free(name);
        free(desc);
        return;
    }
    cat -= 1;

    //Then create a report with the input data
    Report *rep = newReport((IssueType)cat, name, desc, date);
    system("clear");
    printReport(rep);
    insertHashtable(h, rep);

    //Clean up allocated strings
    free(name);
    free(desc);
}

void menuListReports(hashtable h) {
    int count = 0;
    int opt = 0;
    do
    {
        item *rep_list = getHashtableAsList(h, &count);
        printf("Reports listing:\n");
        printList(rep_list);
        printf("\t(0): Previous menu\nSelect an option (0-%d): ", count);
        scanf("%d", &opt);
        system("clear");

        if (opt > 0 && opt <= count ) {
            submenuReportDetails(h, getItem(rep_list, opt)->value);
        }

        freeList(rep_list);
    } while (opt != 0);
}

void menuFilterReports(hashtable h) {
    //Choose the source of reports once and keep showing the same list.
    int source = 0;
    printf("Available filters:\n\t(1): Category\n\t(2): Priority\n\t(3): Status\n\t(4): By Identifier\nSelect an option (1-3 or 0 to go back): ");
    scanf("%d", &source);
    getchar();
    
    //first switch to specify details for the selected filter
    int filter = 0;
    switch (source)
    {
        case 1: //Filter for category
            printf("Choose a category:\n\t(1): Road\n\t(2): Illumination\n\t(3): Garbage Disposal\n\t(4): Public Services\n[1-4]> ");
            scanf("%d", &filter);
            if (filter < 1 || filter > 4) {
                printf("Invalid category, aborting...\n");
                return;
            }
            filter -= 1;
            break;
        case 2: //priority
            printf("Choose a priority:\n\t(1): Low\n\t(2): Below Normal\n\t(3): Normal (Default)\n\t(4): Above Normal\n\t(5): High Priority\n[1-5]> ");
            scanf("%d", &filter);
            if (filter < 1 || filter > 5) {
                printf("Invalid priority number, aborting...\n");
                return;
            }
            filter -= 3;
            break;
        case 3: //status
            printf("Choose status:\n\t(1): Open\n\t(2): Ongoing\n\t(3): Closed\n[1-3]> ");
            scanf("%d", &filter);
            if (filter < 1 || filter > 3) {
                printf("Invalid status, aborting...\n");
                return;
            }
            filter -= 1;
            break;
        case 4: //and single search by id.
            printf("Insert exact report ID (or 0 to go back): ");
            char *id = malloc(sizeof(char) * REPORT_ID_SIZE);
            string_input(id);
            Report *rep;
            if (getHashtableItem(h, id, &rep))
                submenuReportDetails(h, rep);
            else printf("Report not found.\n");
            free(id);
            return;
            break;
        default:
            printf("Invalid option %d, aborting...\n", source);
            return;
    }

    int count = 0;
    int opt = 0;
    do
    {
        item *rep_list = NULL;
        //Second switch to rebuild the list in case a report gets deleted
        switch (source)
        {
            case 1: rep_list = searchHashtableByCategory(h, (IssueType)filter, &count);
                break;
            case 2: rep_list = searchHashtableByPriority(h, (IssueUrgency)filter, &count);
                break;
            case 3: rep_list = searchHashtableByState(h, (IssueState)filter, &count);
                break;
        }

        printf("Search results:\n");
        printList(rep_list);
        printf("\t(0): Previous menu\nSelect an option (0-%d): ", count);
        scanf("%d", &opt);
        system("clear");

        if (opt > 0 && opt <= count ) {
            submenuReportDetails(h, getItem(rep_list, opt)->value);
        }

        freeList(rep_list);
    } while (opt != 0);
}

void menuOverview(hashtable h) {
    //loop through all reports, count all categories, states and most frequent reports per category.
    int num_reports = 0;
    int categories[4];
    int states[3];
    int max = 0, max_index;
    item *rep_list = getHashtableAsList(h, &num_reports), *curr = rep_list;

    //collect overview data from each report
    while (curr)
    {
        categories[curr->value->category]++;
        states[curr->value->state]++;

        curr = curr->next;
    }

    //find maximum and save it's index
    for (int i = 0; i < 4; i++) {
        if (categories[i] > max) {
            max = categories[i];
            max_index = i;
        }
    }
    
    printf("Overview:\n\tTotal Reports: %d\n\tNumber of reports per category:", num_reports);
    printf("\n\t\t-Road related: %d\n\t\t-Street Illumination: %d\n\t\t-Garbage Disposal: %d\n\t\t-Public Services: %d\n\tNumber of reports per status:", categories[0], categories[1], categories[2], categories[3]);
    printf("\n\t\t-Open: %d\n\t\t-Ongoing: %d\n\t\t-Closed: %d\n\tMost problematic category: ", states[0], states[1], states[2]);

    char *m_type;
    switch (max_index)
    {
        case 0: m_type = "Road Maintenance";
        break;
        case 1: m_type = "Illumination";
        break;
        case 2: m_type = "Garbage Disposal";
        break;
        case 3: m_type = "Public Services";
        break;
    }
    printf("%s\n", m_type);

    freeList(rep_list);
}