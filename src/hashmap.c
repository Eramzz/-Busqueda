#include "hashmap.h"
#include "document.h"
#include "query.h"
#include <ctype.h>
#include <stdio.h>

HashMap* hashmapCreate(int size) {
    HashMap* map = malloc(sizeof(HashMap));
    if (!map) return NULL;
    
    map->buckets = calloc(size, sizeof(HashMapEntry*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->size = size;
    map->count = 0;
    return map;
}

// Simple hash function (djb2)
unsigned int hashmapHash(char* key, int size) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash % size;
}

// Create document ID list
DocumentIdList* documentIdListCreate() {
    DocumentIdList* list = malloc(sizeof(DocumentIdList));
    if (!list) return NULL;
    
    list->head = NULL;
    list->count = 0;
    return list;
}

// Check if document ID exists in list
int documentIdListContains(DocumentIdList* list, int documentId) {
    if (!list) return 0;
    
    DocumentIdNode* current = list->head;
    while (current) {
        if (current->documentId == documentId) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Append document ID to list
void documentIdListAppend(DocumentIdList* list, int documentId) {
    if (!list) return;
    
    // Check if already exists
    if (documentIdListContains(list, documentId)) {
        return;
    }
    
    DocumentIdNode* node = malloc(sizeof(DocumentIdNode));
    if (!node) return;
    
    node->documentId = documentId;
    node->next = NULL;
    
    if (list->head == NULL) {
        list->head = node;
    } else {
        DocumentIdNode* current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = node;
    }
    list->count++;
}

// Free document ID list
void documentIdListFree(DocumentIdList* list) {
    if (!list) return;
    
    DocumentIdNode* current = list->head;
    while (current) {
        DocumentIdNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

// Add key-value pair to hashmap
void hashmapPut(HashMap* map, char* key, int documentId) {
    if (!map || !key) return;
    
    unsigned int index = hashmapHash(key, map->size);
    HashMapEntry* entry = map->buckets[index];
    
    // Look for existing entry
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            // Key exists, add document ID to list
            documentIdListAppend(entry->documentIds, documentId);
            return;
        }
        entry = entry->next;
    }
    
    // Create new entry
    entry = malloc(sizeof(HashMapEntry));
    if (!entry) return;
    
    entry->key = malloc(strlen(key) + 1);
    strcpy(entry->key, key);
    
    entry->documentIds = documentIdListCreate();
    documentIdListAppend(entry->documentIds, documentId);
    
    entry->next = map->buckets[index];
    map->buckets[index] = entry;
    map->count++;
}

// Get document list for key
DocumentIdList* hashmapGet(HashMap* map, char* key) {
    if (!map || !key) return NULL;
    
    unsigned int index = hashmapHash(key, map->size);
    HashMapEntry* entry = map->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->documentIds;
        }
        entry = entry->next;
    }
    
    return NULL;
}

// Print hashmap contents
void hashmapPrint(HashMap* map) {
    if (!map) return;
    
    printf("HashMap contents (%d entries):\n", map->count);
    for (int i = 0; i < map->size; i++) {
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            printf("  '%s' -> ", entry->key);
            DocumentIdNode* docNode = entry->documentIds->head;
            while (docNode) {
                printf("%d", docNode->documentId);
                if (docNode->next) printf(", ");
                docNode = docNode->next;
            }
            printf("\n");
            entry = entry->next;
        }
    }
}

// Free hashmap
void hashmapFree(HashMap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->size; i++) {
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            HashMapEntry* temp = entry;
            entry = entry->next;
            
            free(temp->key);
            documentIdListFree(temp->documentIds);
            free(temp);
        }
    }
    
    free(map->buckets);
    free(map);
}

// Normalize word (lowercase, remove punctuation)
char* normalizeWord(char* word) {
    if (!word) return NULL;
    
    int len = strlen(word);
    char* normalized = malloc(len + 1);
    int j = 0;
    
    for (int i = 0; i < len; i++) {
        if (isalnum(word[i])) {
            normalized[j++] = tolower(word[i]);
        }
    }
    normalized[j] = '\0';
    
    // Return NULL for empty words
    if (j == 0) {
        free(normalized);
        return NULL;
    }
    
    return normalized;
}

// Extract words from text and add to reverse index
void extractWordsFromText(char* text, ReverseIndex* index, int documentId) {
    if (!text || !index) return;
    
    char* textCopy = malloc(strlen(text) + 1);
    strcpy(textCopy, text);
    
    char* token = strtok(textCopy, " \t\n\r.,!?;:()[]{}\"'-");
    while (token) {
        char* normalized = normalizeWord(token);
        if (normalized && strlen(normalized) > 0) {
            hashmapPut(index->wordToDocuments, normalized, documentId);
            free(normalized);
        }
        token = strtok(NULL, " \t\n\r.,!?;:()[]{}\"'-");
    }
    
    free(textCopy);
}

// Create reverse index
ReverseIndex* reverseIndexCreate() {
    ReverseIndex* index = malloc(sizeof(ReverseIndex));
    if (!index) return NULL;
    
    // Use a reasonable size for the hashmap
    index->wordToDocuments = hashmapCreate(1000);
    if (!index->wordToDocuments) {
        free(index);
        return NULL;
    }
    
    return index;
}

// Build reverse index from documents
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents) {
    if (!index || !documents) return;
    
    printf("Building reverse index...\n");
    
    Document* current = documents->head;
    int processed = 0;
    
    while (current) {
        // Extract words from title
        extractWordsFromText(current->title, index, current->id);
        
        // Extract words from body
        extractWordsFromText(current->body, index, current->id);
        
        processed++;
        if (processed % 10 == 0) {
            printf("Processed %d documents...\n", processed);
        }
        
        current = current->next;
    }
    
    printf("Reverse index built successfully (%d entries).\n", index->wordToDocuments->count);
}

// Search using reverse index
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments) {
    if (!index || !query || !allDocuments) return NULL;
    
    DocumentsList* results = documentsListCreate();
    if (!results) return NULL;
    
    // Handle empty query
    if (query->itemCount == 0) {
        return results;
    }
    
    // Get documents for first include term
    QueryItem* currentItem = query->head;
    DocumentIdList* candidateDocuments = NULL;
    
    // Find first include term
    while (currentItem && currentItem->isExclude) {
        currentItem = currentItem->next;
    }
    
    if (!currentItem) {
        // No include terms, return empty results
        return results;
    }
    
    // Get documents containing the first include term
    candidateDocuments = hashmapGet(index->wordToDocuments, currentItem->word);
    if (!candidateDocuments) {
        return results; // No documents contain this word
    }
    
    // Create a list of candidate document IDs
    int* candidateIds = malloc(candidateDocuments->count * sizeof(int));
    int candidateCount = 0;
    
    DocumentIdNode* docNode = candidateDocuments->head;
    while (docNode) {
        candidateIds[candidateCount++] = docNode->documentId;
        docNode = docNode->next;
    }
    
    // Filter candidates based on all other query terms
    currentItem = query->head;
    while (currentItem) {
        if (currentItem == query->head) {
            // Skip first item (already processed)
            currentItem = currentItem->next;
            continue;
        }
        
        DocumentIdList* termDocuments = hashmapGet(index->wordToDocuments, currentItem->word);
        
        if (currentItem->isExclude) {
            // Remove documents that contain excluded terms
            for (int i = 0; i < candidateCount; i++) {
                if (candidateIds[i] != -1 && termDocuments && 
                    documentIdListContains(termDocuments, candidateIds[i])) {
                    candidateIds[i] = -1; // Mark as excluded
                }
            }
        } else {
            // Keep only documents that contain included terms
            for (int i = 0; i < candidateCount; i++) {
                if (candidateIds[i] != -1 && 
                    (!termDocuments || !documentIdListContains(termDocuments, candidateIds[i]))) {
                    candidateIds[i] = -1; // Mark as excluded
                }
            }
        }
        
        currentItem = currentItem->next;
    }
    
    // Create result list from remaining candidates
    for (int i = 0; i < candidateCount; i++) {
        if (candidateIds[i] != -1) {
            // Find the document with this ID
            Document* doc = allDocuments->head;
            while (doc) {
                if (doc->id == candidateIds[i]) {
                    // Create shallow copy
                    Document* docCopy = malloc(sizeof(Document));
                    *docCopy = *doc;
                    docCopy->next = NULL;
                    
                    documentsListAppend(results, docCopy);
                    break;
                }
                doc = doc->next;
            }
        }
    }
    
    free(candidateIds);
    return results;
}

// Serialize reverse index to file
int reverseIndexSerialize(ReverseIndex* index, char* filename) {
    if (!index || !filename) return 0;
    
    FILE* file = fopen(filename, "w");
    if (!file) return 0;
    
    HashMap* map = index->wordToDocuments;
    
    for (int i = 0; i < map->size; i++) {
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            fprintf(file, "%s", entry->key);
            
            DocumentIdNode* docNode = entry->documentIds->head;
            while (docNode) {
                fprintf(file, " %d", docNode->documentId);
                docNode = docNode->next;
            }
            fprintf(file, "\n");
            
            entry = entry->next;
        }
    }
    
    fclose(file);
    return 1;
}

// Deserialize reverse index from file
ReverseIndex* reverseIndexDeserialize(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    
    ReverseIndex* index = reverseIndexCreate();
    if (!index) {
        fclose(file);
        return NULL;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        char* word = strtok(line, " ");
        if (!word) continue;
        
        char* docIdStr = strtok(NULL, " ");
        while (docIdStr) {
            int docId = atoi(docIdStr);
            if (docId > 0) {
                hashmapPut(index->wordToDocuments, word, docId);
            }
            docIdStr = strtok(NULL, " ");
        }
    }
    
    fclose(file);
    return index;
}

// Free reverse index
void reverseIndexFree(ReverseIndex* index) {
    if (!index) return;
    
    hashmapFree(index->wordToDocuments);
    free(index);
}