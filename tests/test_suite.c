#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "hash.h"
#include "report.h"
//#include "menu_impl.h"
#include "overview.h"

#define HASH_SIZE 20
#define BUF_SIZE 256

#pragma region Utility functions

/// @brief Replace newlines in the given string with a terminator.
/// @param s String to operate on.
static void replaceNewLine(char *s) {
    int n = strlen(s);
    if (s && s[n-1] == 'n') s[n-1] = 0;
}

/// @brief Reads an overview from a file.
/// @param filename Filename string.
/// @return Pointer to the overview created from the file data.
Overview *getOverviewFromFile(char *filename) {
    FILE *over_file = fopen(filename, "r");
    if (!over_file) {
        fprintf(stderr, "Can't open file %s, ", filename);
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char buffer[BUF_SIZE];
    Overview *over = newOverview();
    int most_freq_type;
    if (fgets(buffer, BUF_SIZE, over_file)) {
        sscanf(buffer, "%d", &over->total_reports);
    }
    if (fgets(buffer, BUF_SIZE, over_file)) {
        sscanf(buffer, "%d %d %d %d", &over->categories[0], &over->categories[1], &over->categories[2], &over->categories[3]);
    }
    if (fgets(buffer, BUF_SIZE, over_file)) {
        sscanf(buffer, "%d %d %d", &over->states[0], &over->states[1], &over->states[2]);
    }
    if (fgets(buffer, BUF_SIZE, over_file)) {
        sscanf(buffer, "%d", &most_freq_type);
        over->most_frequent = (IssueType)most_freq_type;
    }

    fclose(over_file);
    return over;
}

/// @brief Reads reports from a file (until EOF) and returns them in a list.
/// @param filename File to open.
/// @return Pointer to the root of the list.
item *getReportsFromFile(char *filename) {
    FILE *rep_file = fopen(filename, "r");
    if (!rep_file) {
        fprintf(stderr, "Can't open file %s, ", filename);
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char name[BUF_SIZE], desc[BUF_SIZE], datebuf[BUF_SIZE], typebuf[BUF_SIZE];
    item *root = NULL, *tail = NULL;

    while (1) {
        //Read data in sequence, or break if EOF.
        if (!fgets(name, BUF_SIZE, rep_file)) break;
        if (!fgets(desc, BUF_SIZE, rep_file)) break;
        if (!fgets(datebuf, BUF_SIZE, rep_file)) break;
        if (!fgets(typebuf, BUF_SIZE, rep_file)) break;

        replaceNewLine(name);
        replaceNewLine(desc);
        replaceNewLine(datebuf);
        replaceNewLine(typebuf);

        struct tm* date = malloc(sizeof(struct  tm));
        sscanf(datebuf, "%d:%d %d/%d/%d", &date->tm_hour, &date->tm_min, &date->tm_mday, &date->tm_mon, &date->tm_year);
        date->tm_mon -= 1;
        date->tm_year -= 1900;
        date->tm_wday -=1;

        int category = atoi(typebuf);

        Report *rep = newReport(category, name, desc, date);
        item *node = calloc(1, sizeof(item));
        node->value = rep;
        if (!root) root = tail = node;
        else {
            tail->next = node;
            tail = node;
        }
    }
    
    fclose(rep_file);
    return root;
}

/// @brief Creates a copy of the given report.
/// @param rep Report to copy.
/// @return Pointer to copied report.
Report *copyReport(Report *rep) {
    Report *out = malloc(sizeof(Report));
    out->id = rep->id;
    out->issuer_name = rep->issuer_name;
    out->category = rep->category;
    out->desc = rep->desc;
    out->time = rep->time;
    out->state = rep->state;
    out->priority = rep->priority;
    return out;
}

#pragma endregion

/// @brief Tests initialization of a hashtable.
void testInit() {
    hashtable h = newHashtable(HASH_SIZE);
    assert(h);

    deleteHashtable(h);
}

/// @brief Tests correct insertion of a series of reports into a hashtable.
void testInsert() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        assert(insertHashtable(h, curr->value));

        Report *test = NULL;
        getHashtableItem(h, curr->value->id, &test);
        assert(test);

        curr = curr->next;
    }

    freeList(reports);
    deleteHashtable(h);
}

void testDeletion() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    //Create a copy of the first report
    Report *deleted = deleteHashtableItem(h, reports->value->id);
    assert(deleted);
    assert(!getHashtableItem(h, deleted->id, NULL));

    freeList(reports);
    deleteHashtable(h);
}

/// @brief Tests searching for an existing and a non-existing report within the hashtable.
void testSearch() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    //Create a new report that doesn't exist in the hashtable;
    time_t curtime = time(NULL);
    Report* nonexisting = newReport(PublicServices, "Nessuno", "Non presente nell'hashtable", gmtime(&curtime));
    
    Report* existing = NULL;

    //Check if a non-existing report is found or if an existing one hasn't.
    assert(getHashtableItem(h, reports->value->id, &existing));
    assert(getHashtableItem(h, nonexisting->id, &existing) == 0);

    deleteReport(nonexisting);
    freeList(reports);
    deleteHashtable(h);
}

/// @brief Test a filtered search based on category
void testFilter() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    IssueType category = PublicServices;
    item *filtered_reports = NULL;
    filtered_reports = curr = searchHashtableByCategory(h, category, NULL);
    while (curr) {
        assert(curr->value->category == category);
        curr = curr->next;
    }

    freeList(reports);
    freeList(filtered_reports);
    deleteHashtable(h);
}

/// @brief Test changing status of a stored report.
void testState() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    //Fetch report
    Report *test_rep = NULL;
    getHashtableItem(h, reports->value->id, &test_rep);
    
    //Save old status and set a new one
    IssueState oldstate = test_rep->state, newstate = Closed;
    setReportState(test_rep, newstate);

    //Fetch the same report again and check if status has changed
    getHashtableItem(h, reports->value->id, &test_rep);
    assert(test_rep->state != oldstate && test_rep->state == newstate);
    
    freeList(reports);
    deleteHashtable(h);
}

/// @brief Test changing priority of a stored report
void testPriority() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    //Fetch report
    Report *test_rep = NULL;
    getHashtableItem(h, reports->value->id, &test_rep);
    
    //Save old priority and set a new one
    IssueUrgency oldpriority = test_rep->priority, newpriority = High;
    setReportUrgency(test_rep, newpriority);

    //Fetch the same report again and check if priority has changed
    getHashtableItem(h, reports->value->id, &test_rep);
    assert(test_rep->priority != oldpriority && test_rep->priority == newpriority);
    
    freeList(reports);
    deleteHashtable(h);
}

/// @brief Test generating an overview
void testOverview() {
    hashtable h = newHashtable(HASH_SIZE);

    item *reports = NULL, *curr = NULL;
    reports = curr = getReportsFromFile("tests/reports.txt");
    while (curr) {
        insertHashtable(h, curr->value);
        curr = curr->next;
    }

    //Load expected overview output from the input reports
    Overview *pregenerated = getOverviewFromFile("tests/overview.txt");
    Overview *generated = generateOverviewFromHashtable(h);

    //Check if all parameters of the two overviews match
    assert(pregenerated->total_reports == generated->total_reports);
    for (int i = 0; i < 4; i++)
        assert(pregenerated->categories[i] == generated->categories[i]);
    for (int i = 0; i < 3; i++)
        assert(pregenerated->states[i] == generated->states[i]);
    assert(pregenerated->most_frequent == generated->most_frequent);

    freeList(reports);
    deleteOverview(pregenerated);
    deleteOverview(generated);
    deleteHashtable(h);
}

int main() {
    testInit();
    testInsert();
    testDeletion();
    
    testSearch();
    testFilter();
    
    testState();
    testPriority();

    testOverview();

    printf("All tests successful.\n");
    return 0;
}