#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

/// @brief Creates a new hashtable with the given size.
/// @param size Number of internal lists to handle in the hashtable.
/// @return A pointer to the new hashtable object, or NULL if unable to initialize.
hashtable newHashtable(int size) {
    hashtable h = (struct hash *) malloc(sizeof(struct hash));
    if (h == NULL) return NULL;

    h->size = size;
    h->table = (item **) calloc(size, sizeof(item *));
    if (h->table == NULL) {
        free(h);
        return NULL;
    }

    return h;
}

/// @brief Inserts a report into the given hashtable
/// @param h Hashtable to operate on.
/// @param report Report to store.
/// @return 0 if entry is already present in the hashtable, otherwise 1.
int insertHashtable(hashtable h, Report *report) {
    int idx;
    item *curr, *prev = NULL;

    //Find table index from the report's hash
    idx = hashFunction(report->id, h->size);
    curr = h->table[idx];

    //Iterate that list to find equal entries.
    while (curr) {
        if (strcmp(curr->value->id, report->id) == 0) return 0;
        prev = curr;
        curr = curr->next;
    }

    //No duplicates, handle conflicts by placing reports with the same index further down the list.
    item* entry = newItem(report);
    if (!prev) h->table[idx] = entry;
    else prev->next = entry;

    return 1;
}

/// @brief Fetches an item from the dictionary with the given key.
/// @param h Hashtable to operate on.
/// @param key Identifier to the stored value.
/// @param outReport Pointer to the report if contained in the dictionary. Can be NULL if not found.
/// @return 0 if the report was not found, otherwise 1.
int getHashtableItem(hashtable h, char *key, Report *outReport) {
    int idx;
    item *elem;

    idx = hashFunction(key, h->size);
    elem = h->table[idx];

    //While the list node is valid.
    while (elem) { //can be changed to just elem here. (elem->value->id)
        if (strcmp(elem->value->id, key) == 0) {
            outReport = &elem->value;
            return 1;
        }
        //elem->next = malloc(sizeof(item));
        elem = elem->next;
    }

    return 0;
}

/// @brief Deletes an item from the hashtable with the given key.
/// @param h Hashtable to operate on.
/// @param key Identifier for the stored value.
/// @return A pointer to the report (erased from the dictionary) or NULL if not found.
Report *deleteHashtableItem(hashtable h, char *key) {
    //Find index of the list where the report is contained
    int idx = hashFunction(key, h->size);
    item *prev = NULL, *curr = h->table[idx];

    //Loop the list until a report with the matching key is found.
    while (curr) {
        if (!strcmp(curr->value->id, key)) {
            //Prepare a stand-alone pointer to the report
            Report *out = malloc(sizeof(Report));
            if (!out) return NULL;
            out = curr->value;

            //Unlink list nodes if there are any
            if (!prev) h->table[idx] = curr->next;
            else prev->next = curr->next;
            free(curr);

            return out;
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL; 
}

/// @brief Deletes any node in a list. Used by hashtable delete method
/// @param p Root node of the list to delete.
static void deleteList(item *p) {
    item *nextNode;
    while (p != NULL) {
        nextNode = p->next;
        free(p);
        p = nextNode;
    }
}

/// @brief Deallocates the entire hashtable along with any list contained within.
/// @param h Hashtable to delete.
void deleteHashtable(hashtable h) {
    for (int i = 0; i < h->size; i++) {
        deleteList(h->table[i]);
    }
    free(h->table);
    free(h);
}

/// @brief Allocates a new dictionary kv pair. Used by hashtable insert method.
/// @param value The report to create the pair and to derive the key from.
/// @return Pointer to the created key-value pair.
item *newItem(Report *value) {
    item *temp = calloc(1, sizeof(item));
    if (!temp) return NULL;
    temp->value = value;
    return temp;
}

/// @brief Hashes the given key using the Fowler-Noll-Vo hashing function (FNV-1a, 32 bits). The resulting hash is divided by size, returning the remainder to identify the list index for the hashtable.
/// @param key String identifier for the value
/// @param size Maximum index size
/// @return Hashed index for the given key and size.
unsigned int hashFunction(char *key, int size) {
    unsigned int hash = 2166136261u;
    while (*key) {
        hash ^= (unsigned char) *key++;
        hash *= 16777619u;
    }
    return hash % size;
}

/// @brief Searches the hashmap for reports matching the given category. This function is expensive as it iterates through every list in the hashmap.
/// @param h Hashtable to search in.
/// @param category Category to filter for.
/// @param length Optional pointer to write the length of the returned list to. Can be NULL.
/// @return Pointer to the root node of a list containing the results of the search.
item *searchHashtableByCategory(hashtable h, IssueType category, int *length) {
        int count = 0;
    item *results = NULL, *tail = NULL;
    for (int i = 0; i < h->size; i++)
    {
        item *curr_table = h->table[i];
        while (curr_table)
        {
            if (curr_table->value->category == category) {
                item *node = calloc(1, sizeof(item));
                node->value = curr_table->value;
                if (!results) results = tail = node;
                else {
                    tail->next = node;
                    tail = node;
                }
                count++;
            }
            curr_table = curr_table->next;
        }
    }
    if (length) *length = count;
    return results;
}

/// @brief Searches the hashmap for reports matching the given status. This function is expensive as it iterates through every list in the hashmap.
/// @param h Hashtable to search in.
/// @param state Status to filter for.
/// @param length Optional pointer to write the length of the returned list to. Can be NULL.
/// @return Pointer to the root node of a list containing the results of the search.
item *searchHashtableByState(hashtable h, IssueState state, int *length) {
        int count = 0;
    item *results = NULL, *tail = NULL;
    for (int i = 0; i < h->size; i++)
    {
        item *curr_table = h->table[i];
        while (curr_table)
        {
            if (curr_table->value->state == state) {
                item *node = calloc(1, sizeof(item));
                node->value = curr_table->value;
                if (!results) results = tail = node;
                else {
                    tail->next = node;
                    tail = node;
                }
                count++;
            }
            curr_table = curr_table->next;
        }
    }
    if (length) *length = count;
    return results;
}

/// @brief Searches the whole hashmap for reports matching the given priority. This function is expensive as it iterates through every list in the hashmap.
/// @param h Hashtable to search in.
/// @param priority Urgency value to filter for.
/// @param length Optional pointer to write the length of the returned list to. Can be NULL.
/// @return Pointer to the root node of a list containing the results of the search.
item *searchHashtableByPriority(hashtable h, IssueUrgency priority, int *length)
{
    int count = 0;
    item *results = NULL, *tail = NULL;
    for (int i = 0; i < h->size; i++)
    {
        item *curr_table = h->table[i];
        while (curr_table)
        {
            if (curr_table->value->priority == priority) {
                item *node = calloc(1, sizeof(item));
                node->value = curr_table->value;
                if (!results) results = tail = node;
                else {
                    tail->next = node;
                    tail = node;
                }
                count++;
            }
            curr_table = curr_table->next;
        }
    }
    if (length) *length = count;
    return results;
}

/// @brief Returns all the reports in the hashmap as a single list and optionally, it's lenth. This function is expensive as it iterates through every list in the hashmap.
/// @param h Hashtable to return as list.
/// @param length Optional pointer to write the length of the returned list to. Can be NULL.
/// @return A pointer to the first item in the list.
item *getHashtableAsList(hashtable h, int *length)
{
    int count = 0;
    item *results = NULL, *tail = NULL;
    for (int i = 0; i < h->size; i++)
    {
        item *curr_table = h->table[i];
        while (curr_table)
        {
            item *node = calloc(1, sizeof(item));
            node->value = curr_table->value;
            if (!results) results = tail = node;
            else {
                tail->next = node;
                tail = node;
            }
            count++;
            curr_table = curr_table->next;
        }
    }
    if (length) *length = count;
    return results;
}