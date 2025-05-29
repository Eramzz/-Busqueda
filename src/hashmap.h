
#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

typedef struct DocumentIdNode {
    int documentId; //Per sorting
    struct DocumentIdNode* next;
} DocumentIdNode;

typedef struct DocumentIdList {
    DocumentIdNode* head;
    int count;
} DocumentIdList;

// Hash map entry
typedef struct HashMapEntry {
    char* key; //palabra indice
    DocumentIdList* documentIds; //lista que tine la palabra
    struct HashMapEntry* next; //evitar coalicions
} HashMapEntry;

// Hash map structure
typedef struct HashMap {
    HashMapEntry** buckets; //array de punters
    int size; //tamany
    int count;
} HashMap;

// Reverse index structure
typedef struct ReverseIndex {
    HashMap* wordToDocuments;
} ReverseIndex;

// declarations
HashMap* hashmapCreate(int size);
unsigned int hashmapHash(char* key, int size);
void hashmapPut(HashMap* map, char* key, int documentId);
DocumentIdList* hashmapGet(HashMap* map, char* key);
void hashmapFree(HashMap* map);
void hashmapPrint(HashMap* map);

//Amb llistes ID
DocumentIdList* documentIdListCreate();
void documentIdListAppend(DocumentIdList* list, int documentId);
int documentIdListContains(DocumentIdList* list, int documentId);
void documentIdListFree(DocumentIdList* list);

//Reverse index
ReverseIndex* reverseIndexCreate();
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents);
void reverseIndexFree(ReverseIndex* index);
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments);

// Serializacion
int reverseIndexSerialize(ReverseIndex* index, char* filename);
ReverseIndex* reverseIndexDeserialize(char* filename);

// paraula to lower
char* normalizeWord(char* word);
void extractWordsFromText(char* text, ReverseIndex* index, int documentId);

#endif
