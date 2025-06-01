#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H
#include "hashmap.h"

//Estructura reverse index
typedef struct ReverseIndex {
    HashMap* wordToDocuments;  //hashmap relaciona palabras con listas de documentos
} ReverseIndex;

char* normalizeWord(char* word); //Función normaliza una palabra eliminando caracteres no deseados o pasándola a min
void extractWordsFromText(char* text, ReverseIndex* index, int documentId); //Función extrae palabras de un texto y las añade al reverse index con el ID respectivo
ReverseIndex* reverseIndexCreate(); //Función crea nuevo reverse index
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents); //Función construye el índice invertido a partir de lista de documentos
void reverseIndexFree(ReverseIndex* index); //LIbera memoria reverse index
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments); //Función busca documentos en el reverse index segun la query comparando con los docummentos
//Serializa el índice y lo guarda en un archivo.
int reverseIndexSerialize(ReverseIndex* index, char* filename); //Funciones serialización para guardar y cargar el índice desde un archivo
ReverseIndex* reverseIndexDeserialize(char* filename); // Función deserializa el índice desde un archivo, reconstruyendo su estructura

#endif //REVERSE_INDEX_H
