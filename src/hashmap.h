// hashmap.h
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

// Hash map entry
typedef struct HashMapEntry {
    char* key;
    DocumentIdList* documentIds;
    struct HashMapEntry* next;
} HashMapEntry;

// Hash map structure
typedef struct HashMap {
    HashMapEntry** buckets;
    int size;
    int count;
} HashMap;



// Function declarations
HashMap* hashmapCreate(int size);
unsigned int hashmapHash(char* key, int size);
void hashmapPut(HashMap* map, char* key, int documentId);
DocumentIdList* hashmapGet(HashMap* map, char* key);
void hashmapFree(HashMap* map);
void hashmapPrint(HashMap* map);

#endif
