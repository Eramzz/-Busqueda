// document.c
#include "document.h"
#include <ctype.h>

// Parsea un documento desde un archivo
Document* documentDeserialize(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return NULL;

    Document* doc = malloc(sizeof(Document));
    if (!doc) {
        fclose(file);
        return NULL;
    }

    doc->links = NULL;
    doc->next = NULL;
    doc->relevance = 0.0;

    // Lee el ID del documento
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        free(doc);
        fclose(file);
        return NULL;
    }

    // Lee el título
    char titleBuffer[1024];
    if (!fgets(titleBuffer, sizeof(titleBuffer), file) {
        free(doc);
        fclose(file);
        return NULL;
    }

    // Elimina el salto de línea del título
    size_t titleLen = strlen(titleBuffer);
    if (titleLen > 0 && titleBuffer[titleLen - 1] == '\n') {
        titleBuffer[titleLen - 1] = '\0';
        titleLen--;
    }

    doc->title = malloc(titleLen + 1);
    strcpy(doc->title, titleBuffer);

    // Lee el cuerpo (resto del archivo)
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Omite el ID y el título
    char temp[1024];
    fgets(temp, sizeof(temp), file); // Línea del ID
    fgets(temp, sizeof(temp), file); // Línea del título

    long bodyStart = ftell(file);
    long bodySize = fileSize - bodyStart;

    doc->body = malloc(bodySize + 1);
    if (!doc->body) {
        free(doc->title);
        free(doc);
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(doc->body, 1, bodySize, file);
    doc->body[bytesRead] = '\0';

    // Parsea los enlaces del cuerpo
    char* bodyPtr = doc->body;
    while ((bodyPtr = strchr(bodyPtr, '[')) != NULL) {
        char* linkEnd = strchr(bodyPtr, ']');
        char* linkStart = strchr(bodyPtr, '(');
        char* linkClose = strchr(bodyPtr, ')');

        if (linkEnd && linkStart && linkClose &&
            linkStart > linkEnd && linkClose > linkStart) {

            // Extrae el ID del enlace
            char linkIdStr[32];
            int linkIdLen = linkClose - linkStart - 1;
            if (linkIdLen > 0 && linkIdLen < 32) {
                strncpy(linkIdStr, linkStart + 1, linkIdLen);
                linkIdStr[linkIdLen] = '\0';

                int linkId = atoi(linkIdStr);
                if (linkId > 0) {
                    linkAppend(&doc->links, linkId);
                }
            }
            bodyPtr = linkClose + 1;
        } else {
            bodyPtr++;
        }
    }

    fclose(file);
    return doc;
}


// Imprime los detalles de un documento
void documentPrint(Document* doc) {
    if (!doc) return;

    printf("\n=== Documento %d ===\n", doc->id);
    printf("Título: %s\n", doc->title);
    printf("Relevancia: %.2f\n", doc->relevance);
    printf("\nCuerpo:\n%s\n", doc->body);

    if (doc->links) {
        printf("\nEnlaces a documentos: ");
        Link* current = doc->links;
        while (current) {
            printf("%d", current->id);
            if (current->next) printf(", ");
            current = current->next;
        }
        printf("\n");
    }
    printf("\n");
}

// Libera la memoria de un documento
void documentFree(Document* doc) {
    if (!doc) return;
    free(doc->title);
    free(doc->body);
    linkFree(doc->links);
    free(doc);
}