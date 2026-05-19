#include <stdio.h>
#include <stdlib.h>
#include "report.h"
#include "menu.h"

#include "hash.h"

int main() {
    hashtable table = newHashtable(20);
    srand(time(NULL));
    
    /* //Example for creating a new report
    time_t curtime = time(NULL);
    Report* rep = newReport(PublicServices, "Mario Rossi", "Disservizio alla fornitura di gas", gmtime(&curtime));
    printReport(rep);
    printf("Index for the given report in a size %d hashtable results in %d\n", 20, hashFunction(rep->id, 20));
    insertHashtable(table, rep);
    */

    doMenu(table);
    
    deleteHashtable(table);
}