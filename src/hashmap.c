#include "hashmap.h"
#include "document.h"
#include "query.h"
#include <ctype.h>
#include <stdio.h>

//Función crea y inicializa un hashmap y reserva memoria para su estructura y los buckets
HashMap* hashmapCreate(int size) {
    HashMap* map = malloc(sizeof(HashMap)); //reserva memoria para la estructura del hashmap
    if (!map){
        return NULL; //si falla al reservar memoria, devuelve null
    }

    map->buckets = calloc(size, sizeof(HashMapEntry*)); //inicializa el array de buckets a NULL
    if (!map->buckets) { //si falla al reservar memoria para los buckets
        free(map); //libera la memoria del hashmap
        return NULL; //devuelve null
    }

    map->size = size; //guarda el tamaño total del hashmap
    map->count = 0; //inicializa el contador de elementos
    return map; //devuelve el hashmap inicializado
}

//Función hash usando el algoritmo djb2, calcula el índice basado en la clave y el tamaño del hashmap
unsigned int hashmapHash(char* key, int size) {
    unsigned long hash = 5381; //valor inicial del hash
    int c;

    while ((c = *key++)) { //recorre cada carácter de la clave
        hash = ((hash << 5) + hash) + c; // plica función hash: hash * 33 + c
    }

    return hash % size; //devuelve el índice final limitado al tamaño del hashmap
}

//Función inserta una clave en el hashmap y asocia un id de documento. si la clave ya existe, añade el id a su lista
void hashmapPut(HashMap* map, char* key, int documentId) {
    if (!map || !key) return; //si el mapa o la clave son nulos, no hace nada

    unsigned int index = hashmapHash(key, map->size); //obtiene el índice en el hashmap usando la función hash
    HashMapEntry* entry = map->buckets[index]; //accede a la lista de entradas en ese índice

    //busca si la clave ya existe en esa posición
    while (entry) {
        if (strcmp(entry->key, key) == 0) { //si encuentra la clave
            documentIdListAppend(entry->documentIds, documentId); //añade el id a la lista de documentos
            return; // termina
        }
        entry = entry->next; //avanza al siguiente elemento en la lista
    }

    //si la clave no existe, crea una nueva entrada
    entry = malloc(sizeof(HashMapEntry)); //reserva memoria para la nueva entrada
    if (!entry) return; //si falla la reserva, termina

    entry->key = malloc(strlen(key) + 1); /reserva memoria para copiar la clave
    strcpy(entry->key, key); //copia la clave

    entry->documentIds = documentIdListCreate(); //crea una nueva lista de documentos
    documentIdListAppend(entry->documentIds, documentId); //añade el documento a la lista

    entry->next = map->buckets[index]; //enlaza la nueva entrada al principio de la lista del bucket
    map->buckets[index] = entry; //pone la nueva entrada en el bucket
    map->count++; //incrementa el número total de entradas
}

//Función obtiene la lista de documentos asociada a una clave específica, retorna la lista si la clave está, si no, devuelve null
DocumentIdList* hashmapGet(HashMap* map, char* key) {
    if (!map || !key){
        return NULL; // si el mapa o la clave son nulos, retorna null
    }

    unsigned int index = hashmapHash(key, map->size); //calcula el índice para la clave
    HashMapEntry* entry = map->buckets[index]; //accede a la lista en ese índice

    while (entry) { //recorre las entradas del bucket
        if (strcmp(entry->key, key) == 0) { //si encuentra la clave
            return entry->documentIds; //devuelve la lista de documentos
        }
        entry = entry->next; //avanza al siguiente nodo
    }

    return NULL; /si no encuentra la clave, retorna null
}

//Función imprime el contenido del hashmap: cada clave con sus ids de documentos asociados
void hashmapPrint(HashMap* map) {
    if (!map){
        return; //si el mapa es nulo, no hace nada
    }

    printf("hashmap (%d entradas):\n", map->count); //imprime el número de entradas
    for (int i = 0; i < map->size; i++) { //recorre cada bucket
        HashMapEntry* entry = map->buckets[i];
        while (entry) { //recorre las entradas dentro del bucket
            printf("  '%s' -> ", entry->key); //imprime la clave

            DocumentIdNode* docNode = entry->documentIds->head; //accede a la lista de ids
            while (docNode) {
                printf("%d", docNode->documentId); //imprime el id del documento
                if (docNode->next) printf(", "); //si hay más, añade coma
                docNode = docNode->next; //avanza al siguiente id
            }
            printf("\n"); //nueva línea para la siguiente clave
            entry = entry->next; //avanza a la siguiente entrada del bucket
        }
    }
}

//libera toda la memoria utilizada por el hashmap y sus elementos
void hashmapFree(HashMap* map) {
    if (!map) return; //si el mapa es nulo, no hace nada

    for (int i = 0; i < map->size; i++) { //recorre todos los buckets
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            HashMapEntry* temp = entry; //guarda la entrada actual
            entry = entry->next; //avanza a la siguiente

            free(temp->key); //libera la memoria de la clave
            documentIdListFree(temp->documentIds); //libera la lista de documentos
            free(temp); //libera la entrada completa
        }
    }

    free(map->buckets); //libera el array de buckets
    free(map); //libera la estructura del hashmap
}