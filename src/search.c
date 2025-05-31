#include "search.h"
#include "query.h"

// Limpia una palabra (elimina puntuación y convierte a minúsculas)

// Verifica si un documento contiene una palabra
int documentContainsWord(Document* doc, char* word) {
    if (!doc || !word) return 0;

    // Busca en el título
    char* cleanedTitle = cleanWord(doc->title);
    if (cleanedTitle && strstr(cleanedTitle, word)) {
        free(cleanedTitle);
        return 1;
    }
    free(cleanedTitle);

    // Busca en el cuerpo
    char* cleanedBody = cleanWord(doc->body);
    if (cleanedBody && strstr(cleanedBody, word)) {
        free(cleanedBody);
        return 1;
    }
    free(cleanedBody);

    return 0;
}

// Crea una copia de una lista de documentos
DocumentsList* documentsListCopy(DocumentsList* source) {
    if (!source) return NULL;

    DocumentsList* copy = documentsListCreate();
    if (!copy) return NULL;

    Document* current = source->head;
    while (current) {
        // Copia superficial (referencia los mismos documentos)
        Document* docCopy = malloc(sizeof(Document));
        *docCopy = *current;
        docCopy->next = NULL;

        documentsListAppend(copy, docCopy);
        current = current->next;
    }

    return copy;
}

// Busca documentos que coincidan con la consulta
DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query) {
    if (!documents || !query) return NULL;

    DocumentsList* results = documentsListCreate();
    if (!results) return NULL;

    Document* current = documents->head;

    while (current) {
        int matches = 1; // Asume que el documento coincide hasta que se demuestre lo contrario

        // Verifica todos los ítems de la consulta
        QueryItem* queryItem = query->items;
        while (queryItem && matches) {
            int contains = documentContainsWord(current, queryItem->word);

            if (queryItem->isExclude) {
                // Para términos excluidos, el documento NO debe contener la palabra
                if (contains) {
                    matches = 0;
                }
            } else {
                // Para términos incluidos, el documento DEBE contener la palabra
                if (!contains) {
                    matches = 0;
                }
            }

            queryItem = queryItem->next;
        }

        if (matches) {
            // Crea una copia superficial para los resultados
            Document* docCopy = malloc(sizeof(Document));
            *docCopy = *current;
            docCopy->next = NULL;

            documentsListAppend(results, docCopy);
        }

        current = current->next;
    }

    return results;
}