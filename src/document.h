#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "link.h"

//Estructura DOcument
typedef struct Document {
    int id; //ID documento
    char* title; //puntero a cadena titulo documento
    char* body; //puntero a cadena cuerpo documento
    Link* links; //puntero a lista enlazada de Links para saber a que documentos enlazado, guardando una lista de sus IDs con nodos tipo Link.
    float relevance; //relevancia del documento, para ordenarlo en resultados de búsqueda, ppio es 0.0
    struct Document* next; //puntero al siguiente documento en lista enlazada
} Document;

Document* documentDeserialize(char* path); //Función que lee un archivo de texto y hace un Document en memoria con su info
void documentPrint(Document* doc); //Función imprime los detalles de un documento
void documentFree(Document* doc); //Función libera memoria documento

#endif
