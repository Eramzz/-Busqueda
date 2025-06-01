#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H

#include "document.h"

//Estructura lista  enlazada con un contador de elementos
typedef struct DocumentsList {
    Document* head;  //puntero al primer documento de la lista
    int count;    //num total de documentos en la lista
} DocumentsList;

//Estructura nodo individual de una lista de IDs de documento
typedef struct DocumentIdNode {
    int documentId;    //ID del documento
    struct DocumentIdNode* next; //sig nodo lista
} DocumentIdNode;

//Estructura lista completa de IDs de documentos usa una lista enlazada y lleva la cuenta de los elementos.
typedef struct DocumentIdList {
    DocumentIdNode* head; //puntero al primer nodo
    int count;   //num de IDs almacenados
} DocumentIdList;


DocumentsList* documentsListCreate(); //crea una lista de documentos
void documentsListAppend(DocumentsList* list, Document* doc); //añade un documento a la lista
void documentsListPrint(DocumentsList* list); //imprime la lista de documentos
Document* documentsListGet(DocumentsList* list, int index); //obtiene un documento por índice
void documentsListFree(DocumentsList* list); //libera la memoria de la lista de documentos

DocumentIdList* documentIdListCreate(); // Crea una lista de IDs de documentos
int documentIdListContains(DocumentIdList* list, int documentId); //verifica si un ID de documento existe en la lista
void documentIdListAppend(DocumentIdList* list, int documentId); //añade un ID de documento a la lista
void documentIdListFree(DocumentIdList* list); //libera la memoria de la lista de IDs

#endif //DOCUMENT_LIST_H
