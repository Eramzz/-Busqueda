#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

typedef struct HashMapEntry {
    char* key;  //Clave representa la palabra almacenada
    DocumentIdList* documentIds;  //Lista ID documentos asociados a la palabra
    struct HashMapEntry* next;  //Puntero a sig elemento entrada por si hay colisiones
} HashMapEntry;

//Estructura del mapa hash, guarda entradas
typedef struct HashMap {
    HashMapEntry** buckets;  //Array con buckets que guardan las entradas del hashmap
    int size;  //num total huecos (buckets en hashmap
    int count; //num de elementos guardados en el hashmap
} HashMap;

HashMap* hashmapCreate(int size); //Crea hashmap con tamaño deseado y devuelve el puntero a la estructura inicializada
unsigned int hashmapHash(char* key, int size); //Calcula el hash de una clave y devuelve su posición en el hashmap
void hashmapPut(HashMap* map, char* key, int documentId);
//Inserta una clave y su identificador de documento en el hashmap, si ya existe añade el nuevo ID de documento a la lista existente
DocumentIdList* hashmapGet(HashMap* map, char* key);
//Recupera lista de ID de documentos asociados a una clave específica y devuelve puntero a la lista si la clave esta sino devuelve NULL.
void hashmapFree(HashMap* map); //Libera memoria hashmap y eliminan todas las entradas y el array
void hashmapPrint(HashMap* map); //Imprime contenido del hashmap y enseña claves y sus documentos asociados

#endif//HASHMAP_H
