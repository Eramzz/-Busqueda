#include "hashmap.h"
#include "document.h"
#include "query.h"
#include <ctype.h>
#include <stdio.h>

//Crea y inicializa hashmap y guarda memoria para la estructura y el array
HashMap* hashmapCreate(int size) {
    HashMap* map = malloc(sizeof(HashMap));
    if (!map){
        return NULL; //Si falla al guardar memoria, devuelve NULL
    }

    map->buckets = calloc(size, sizeof(HashMapEntry*)); //Inicializa buckets como NULL
    if (!map->buckets) {
        free(map);
        return NULL;
    }

    map->size = size;
    map->count = 0;
    return map;
}

//Función hash, el algoritmo djb2, Calcula índice basado en la clave y el tamaño hashmap
unsigned int hashmapHash(char* key, int size) {
    unsigned long hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % size;
}

//Inserta clave en hashmap y asocia un ID de documento. Si la clave ya existe, añade el ID del documento a su lista de documentos
void hashmapPut(HashMap* map, char* key, int documentId) {
    if (!map || !key) return;

    unsigned int index = hashmapHash(key, map->size);
    HashMapEntry* entry = map->buckets[index];

    //Busca si la clave ya está en el hashmap
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            documentIdListAppend(entry->documentIds, documentId);
            return;
        }
        entry = entry->next;
    }

    //Crea nueva entrada si la clave no existe
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

//Obtiene lista de documentos asociada a una clave específica hashmap
//Retorna puntero a la lista de documentos si la clave está,sino devuelve NULL
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

//Imprime contenido hashmap. Cada cada clave con los identificadores de documentos asociados a estas
void hashmapPrint(HashMap* map) {
    if (!map) return;

    printf("Hashmap (%d entradas):\n", map->count);
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

//Libera la memoria utilizada hashmap y sus elementos y recorre todos los huecos y libera cada entrada y sus listas de documentos
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