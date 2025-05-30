//
// Created by Dell on 30/05/2025.
//

#include "reverse_index.h"

// Normaliza una palabra (minúsculas, sin puntuación)
char* normalizeWord(char* word) {
    if (!word) return NULL;

    int len = strlen(word);
    char* normalized = malloc(len + 1);
    int j = 0;

    for (int i = 0; i < len; i++) {
        if (isalnum(word[i])) {
            normalized[j++] = tolower(word[i]);
        }
    }
    normalized[j] = '\0';

    // Retorna NULL para palabras vacías
    if (j == 0) {
        free(normalized);
        return NULL;
    }

    return normalized;
}

// Extrae palabras de un texto y las añade al índice invertido
void extractWordsFromText(char* text, ReverseIndex* index, int documentId) {
    if (!text || !index) return;

    char* textCopy = malloc(strlen(text) + 1);
    strcpy(textCopy, text);

    char* token = strtok(textCopy, " \t\n\r.,!?;:()[]{}\"'-");
    while (token) {
        char* normalized = normalizeWord(token);
        if (normalized && strlen(normalized) > 0) {
            hashmapPut(index->wordToDocuments, normalized, documentId);
            free(normalized);
        }
        token = strtok(NULL, " \t\n\r.,!?;:()[]{}\"'-");
    }

    free(textCopy);
}

// Crea un índice invertido
ReverseIndex* reverseIndexCreate() {
    ReverseIndex* index = malloc(sizeof(ReverseIndex));
    if (!index) return NULL;

    index->wordToDocuments = hashmapCreate(1000); // Tamaño razonable
    if (!index->wordToDocuments) {
        free(index);
        return NULL;
    }

    return index;
}

// Construye el índice invertido a partir de documentos
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents) {
    if (!index || !documents) return;

    printf("Construyendo índice invertido...\n");

    Document* current = documents->head;
    int processed = 0;

    while (current) {
        // Procesa el título
        extractWordsFromText(current->title, index, current->id);

        // Procesa el cuerpo
        extractWordsFromText(current->body, index, current->id);

        processed++;
        if (processed % 10 == 0) {
            printf("Procesados %d documentos...\n", processed);
        }

        current = current->next;
    }

    printf("Índice invertido construido (%d entradas).\n", index->wordToDocuments->count);
}

// Busca documentos usando el índice invertido
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments) {
    if (!index || !query || !allDocuments) return NULL;

    DocumentsList* results = documentsListCreate();
    if (!results) return NULL;

    // Consulta vacía
    if (query->itemCount == 0) {
        return results;
    }

    // Procesa términos de búsqueda
    QueryItem* currentItem = query->items;
    DocumentIdList* candidateDocuments = NULL;

    // Encuentra el primer término de inclusión
    while (currentItem && currentItem->isExclude) {
        currentItem = currentItem->next;
    }

    if (!currentItem) {
        return results; // No hay términos de inclusión
    }

    // Obtiene documentos para el primer término
    candidateDocuments = hashmapGet(index->wordToDocuments, currentItem->word);
    if (!candidateDocuments) {
        return results;
    }

    // Filtra documentos según la consulta
    int* candidateIds = malloc(candidateDocuments->count * sizeof(int));
    int candidateCount = 0;

    DocumentIdNode* docNode = candidateDocuments->head;
    while (docNode) {
        candidateIds[candidateCount++] = docNode->documentId;
        docNode = docNode->next;
    }

    currentItem = query->items;
    while (currentItem) {
        DocumentIdList* termDocuments = hashmapGet(index->wordToDocuments, currentItem->word);

        if (currentItem->isExclude) {
            // Excluye documentos con términos prohibidos
            for (int i = 0; i < candidateCount; i++) {
                if (candidateIds[i] != -1 && termDocuments &&
                    documentIdListContains(termDocuments, candidateIds[i])) {
                    candidateIds[i] = -1;
                }
            }
        } else {
            // Incluye solo documentos con términos requeridos
            for (int i = 0; i < candidateCount; i++) {
                if (candidateIds[i] != -1 &&
                    (!termDocuments || !documentIdListContains(termDocuments, candidateIds[i]))) {
                    candidateIds[i] = -1;
                }
            }
        }

        currentItem = currentItem->next;
    }

    // Genera resultados
    for (int i = 0; i < candidateCount; i++) {
        if (candidateIds[i] != -1) {
            Document* doc = allDocuments->head;
            while (doc) {
                if (doc->id == candidateIds[i]) {
                    Document* docCopy = malloc(sizeof(Document));
                    *docCopy = *doc;
                    docCopy->next = NULL;
                    documentsListAppend(results, docCopy);
                    break;
                }
                doc = doc->next;
            }
        }
    }

    free(candidateIds);
    return results;
}

// Serializa el índice invertido a un archivo
int reverseIndexSerialize(ReverseIndex* index, char* filename) {
    if (!index || !filename) return 0;

    FILE* file = fopen(filename, "w");
    if (!file) return 0;

    HashMap* map = index->wordToDocuments;

    for (int i = 0; i < map->size; i++) {
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            fprintf(file, "%s", entry->key);

            DocumentIdNode* docNode = entry->documentIds->head;
            while (docNode) {
                fprintf(file, " %d", docNode->documentId);
                docNode = docNode->next;
            }
            fprintf(file, "\n");

            entry = entry->next;
        }
    }

    fclose(file);
    return 1;
}

// Deserializa el índice invertido desde un archivo
ReverseIndex* reverseIndexDeserialize(char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;

    ReverseIndex* index = reverseIndexCreate();
    if (!index) {
        fclose(file);
        return NULL;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        char* word = strtok(line, " ");
        if (!word) continue;

        char* docIdStr = strtok(NULL, " ");
        while (docIdStr) {
            int docId = atoi(docIdStr);
            if (docId > 0) {
                hashmapPut(index->wordToDocuments, word, docId);
            }
            docIdStr = strtok(NULL, " ");
        }
    }

    fclose(file);
    return index;
}

// Libera la memoria del índice invertido
void reverseIndexFree(ReverseIndex* index) {
    if (!index) return;

    hashmapFree(index->wordToDocuments);
    free(index);
}