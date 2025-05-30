// hashmap.c
#include "hashmap.h"
#include "document.h"
#include "query.h"
#include <ctype.h>
#include <stdio.h>

// Crea un nuevo mapa hash
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

// Función hash (djb2)
unsigned int hashmapHash(char* key, int size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % size;
}

// Añade una clave-valor al mapa hash
void hashmapPut(HashMap* map, char* key, int documentId) {
    if (!map || !key) return;

    unsigned int index = hashmapHash(key, map->size);
    HashMapEntry* entry = map->buckets[index];

    // Busca una entrada existente
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            // La clave existe, añade el ID del documento a la lista
            documentIdListAppend(entry->documentIds, documentId);
            return;
        }
        entry = entry->next;
    }

    // Crea una nueva entrada
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

// Obtiene la lista de documentos asociada a una clave
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

// Imprime el contenido del mapa hash
void hashmapPrint(HashMap* map) {
    if (!map) return;

    printf("Contenido del HashMap (%d entradas):\n", map->count);
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

// Libera la memoria del mapa hash
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