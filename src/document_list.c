#include "document_list.h"
#include <stdio.h>
#include <stdlib.h>

//crea una lista de documentos
DocumentsList* documentsListCreate() {
    DocumentsList* list = malloc(sizeof(DocumentsList)); //reserva memoria para la estructura de la lista
    if (!list) return NULL; //si la reserva falla, retorna null

    list->head = NULL; //inicializa el puntero al primer documento como vacío
    list->count = 0; //inicializa el contador de documentos en cero
    return list; //devuelve puntero a la nueva lista
}


//añade un documento a la lista
void documentsListAppend(DocumentsList* list, Document* doc) {
    if (!list || !doc){
        return; //si la lista o el documento son nulos, no hace nada
    }

    if (list->head == NULL) { //si la lista está vacía
        list->head = doc; //el documento se convierte en el primero de la lista
    } else {
        Document* current = list->head; //empieza desde el primer documento
        while (current->next) { //recorre hasta el final de la lista
            current = current->next;
        }
        current->next = doc; //añade el nuevo documento al final
    }
    list->count++; //incrementa el número de documentos en la lista
}

//imprime la lista de documentos
void documentsListPrint(DocumentsList* list) {
    if (!list || !list->head) { //si la lista es nula o está vacía
        printf("no se han encontrado documentos\n");
        return; //sale de la función
    }

    printf("\ndocumentos disponibles:\n");
    printf("=======\n");

    Document* current = list->head; //empieza desde el primer documento
    int index = 0; //índice para numerar los documentos

    while (current) { //recorre toda la lista
        printf("%d. %s\n", index, current->title); //imprime el índice y el título del documento
        index++; //incrementa el índice
        current = current->next; //pasa al siguiente documento
    }

    printf("\n");
}

// obtiene un documento por índice
Document* documentsListGet(DocumentsList* list, int index) {
    if (!list || index < 0){
        return NULL; //si la lista es nula o el índice es negativo, devuelve null
    }

    Document* current = list->head; //empieza desde el primer documento
    int currentIndex = 0; //contador para posición actual

    while (current && currentIndex < index) { //recorre la lista hasta alcanzar el índice deseado o el final
        current = current->next; //avanza al siguiente documento
        currentIndex++; //incrementa el contador de posición
    }

    return current; //retorna el documento encontrado (o null si no existe en esa posición)
}

// libera la memoria de la lista de documentos
void documentsListFree(DocumentsList* list) {
    if (!list){
        return; //si la lista es nula, no hay nada que liberar
    }

    Document* current = list->head; //empieza desde el primer documento
    while (current) { //recorre toda la lista
        Document* temp = current; //guarda el documento actual temporalmente
        current = current->next; //pasa al siguiente documento
        documentFree(temp); //libera la memoria del documento actual
    }
    free(list); //libera memoria estructura de la lista
}



//crea una lista de ids de documentos
DocumentIdList* documentIdListCreate() {
    DocumentIdList* list = malloc(sizeof(DocumentIdList)); //reserva memoria para la lista de ids
    if (!list){
        return NULL; //si falla devuelve null
    }

    list->head = NULL; //inicializa la cabeza de la lista como vacía
    list->count = 0; //inicializa el contador de elementos en 0
    return list; //devuelve la lista creada
}

//mira si un id de documento existe en la lista
int documentIdListContains(DocumentIdList* list, int documentId) {
    if (!list) return 0; //si la lista no existe, devuelve 0 (no encontrado)

    DocumentIdNode* current = list->head; //comienza desde el primer nodo de la lista
    while (current) { //recorre todos los nodos
        if (current->documentId == documentId) { //si encuentra el id buscado
            return 1; //devuelve 1, lo ha encontrado
        }
        current = current->next; //pasa al sig nodo
    }
    return 0; //no lo encuentra, devuelve 0
}

//añade un id de documento a la lista
void documentIdListAppend(DocumentIdList* list, int documentId) {
    if (!list){
        return; //si la lista no existe, no hace nada
    }

    //nira si ya existe el ID en la lista
    if (documentIdListContains(list, documentId)) {
        return; //si ya está, no lo añade
    }

    DocumentIdNode* node = malloc(sizeof(DocumentIdNode)); //guarda memoria para el nuevo nodo
    if (!node){
        return; //si falla, no hace nada
    }

    node->documentId = documentId; //asigna el ID al nuevo nodo
    node->next = NULL; //el nuevo nodo no apunta a ningún otro

    if (list->head == NULL) {
        list->head = node; //si la lista está vacía, el nuevo nodo es el primero
    } else {
        DocumentIdNode* current = list->head; //empieza desde el primer nodo
        while (current->next) { //recorre hasta el último nodo
            current = current->next;
        }
        current->next = node; //añade el nuevo nodo al final
    }
    list->count++; //suma 1 al contador de nodos
}

//libera la memoria de la lista de ids
void documentIdListFree(DocumentIdList* list) {
    if (!list){
        return; //si la lista no existe, no hace nada
    }

    DocumentIdNode* current = list->head; //apunta al primer nodo de la lista
    while (current) { //recorre todos los nodos
        DocumentIdNode* temp = current; //guarda el nodo actual
        current = current->next; //avanza al siguiente nodo
        free(temp); //libera la memoria del nodo actual
    }
    free(list); //libera la estructura principal de la lista
}