#include "document_list.h"
// Crea una lista de documentos
DocumentsList* documentsListCreate() {
    DocumentsList* list = malloc(sizeof(DocumentsList));
    if (!list) return NULL;
    list->head = NULL;
    list->count = 0;
    return list;
}

// Añade un documento a la lista
void documentsListAppend(DocumentsList* list, Document* doc) {
    if (!list || !doc) return;

    if (list->head == NULL) {
        list->head = doc;
    } else {
        Document* current = list->head;
        while (current->next) {
            current = current->next;
        }
        current->next = doc;
    }
    list->count++;
}

// Imprime la lista de documentos
void documentsListPrint(DocumentsList* list) {
    if (!list || !list->head) {
        printf("No se encontraron documentos.\n");
        return;
    }

    printf("\nDocumentos disponibles:\n");
    printf("====================\n");

    Document* current = list->head;
    int index = 0;
    while (current) {
        printf("%d. %s\n", index, current->title);
        index++;
        current = current->next;
    }
    printf("\n");
}

// Obtiene un documento por índice
Document* documentsListGet(DocumentsList* list, int index) {
    if (!list || index < 0) return NULL;

    Document* current = list->head;
    int currentIndex = 0;

    while (current && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }

    return current;
}

// Libera la memoria de la lista de documentos
void documentsListFree(DocumentsList* list) {
    if (!list) return;

    Document* current = list->head;
    while (current) {
        Document* temp = current;
        current = current->next;
        documentFree(temp);
    }
    free(list);
}



// Crea una lista de IDs de documentos
DocumentIdList* documentIdListCreate() {
    DocumentIdList* list = malloc(sizeof(DocumentIdList));
    if (!list) return NULL;

    list->head = NULL;
    list->count = 0;
    return list;
}

// Verifica si un ID de documento existe en la lista
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

// Añade un ID de documento a la lista
void documentIdListAppend(DocumentIdList* list, int documentId) {
    if (!list) return;

    // Verifica si ya existe
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

// Libera la memoria de la lista de IDs
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