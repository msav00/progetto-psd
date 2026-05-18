#include "report.h"
#pragma once

struct node {
    Report *value;
    struct node *next;
};

typedef struct node item;

struct hash {
    int size;
    item **table;
};

typedef struct hash *hashtable;

//Generic hashtable methods

hashtable newHashtable(int size);
int insertHashtable(hashtable h, Report *report);
int getHashtableItem(hashtable h, char *key, Report *outReport);
Report *deleteHashtableItem(hashtable h, char *key);
void deleteHashtable(hashtable h);

//Search functions

item *searchHashtableByCategory(hashtable h, IssueType category, int *length);
item *searchHashtableByState(hashtable h, IssueState state, int *length);
item *searchHashtableByPriority(hashtable h, IssueUrgency priority, int *length);
item *getHashtableAsList(hashtable h, int *length);

//Utilities

unsigned int hashFunction(char *key, int size);
item *newItem(Report *value);