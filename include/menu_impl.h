#include "hash.h"

void string_input(char *dest);
void printList(item *root);
item *getItem(item *root, int index);

void menuNewReport(hashtable h);
void menuListReports(hashtable h);
void menuFilterReports(hashtable h);
void menuOverview(hashtable h);