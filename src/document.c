#include "document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Link* linkCreate(int id) {
    Link* link = malloc(sizeof(Link));
    if (!link) return NULL;

    link->id = id;
    link->next = NULL;
    return link;
}
void linkAppend(Link** head, int id) {
    if (!head) return; //Significa que es buida
    Link* new_link = linkCreate(id);
    if (!new_link) return;

    if (*head == NULL) {
        *head = new_link;
        return;
    }
    //Mira si ya existeix
    Link* current = *head;
    while (current) {
        if (current->id == id) {
            free(new_link);
            return;
        }
        if (current ->next == NULL) break;
        current = current->next;
    }
    current->next = new_link;
}
void linkFree(Link* head) {
    while (head) {
        Link* temp = head;
        head = head->next;
        free(temp);
    }
}


Document* documentDeserialize(char* pathf) {
    FILE* file = fopen(pathf, "r");
    if (!file) return NULL;

    Document* doc = malloc(sizeof(Document));
    if (!doc) {
        fclose(file);
        return NULL;
    }

    // Inicializar valores
    doc->links = NULL;
    doc->next = NULL;
    doc->relevancia = 0.0f;
    // Leer ID
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        free(doc);
        fclose(file);
        return NULL;
    }

    // Leer título
    char titulbuffer[1024];
    if (!fgets(titulbuffer, sizeof(titulbuffer), file)) {
        free(doc);
        fclose(file);
        return NULL;
    }

    size_t titulen = strlen(titulbuffer);
    if (titulen >0 && titulbuffer[titulen-1] == '\n') {
        titulbuffer[titulen-1] = '\0';
        titulen--;
    }

    doc->title = malloc(titulen+1);
    strcpy(doc->title, titulbuffer);

    //llegir body
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char temp[1024];
    fgets(temp, sizeof(temp), file); //id
    fgets(temp, sizeof(temp), file); //titul

    long body_start = ftell(file);
    long body_size = filesize - body_start;

    doc->body = malloc(body_size+1);
    if (!doc->body) {
        free(doc->title);
        free(doc);
        fclose(file);
        return NULL;
    }

    size_t bytesllegits = fread(doc->body, 1, body_size, file);
    doc->body[bytesllegits] = '\0';

    char* bodyPtr = doc->body;
    while ((bodyPtr = strchr(bodyPtr, '[')) != NULL) {
        char* linkEnd = strchr(bodyPtr, ']');
        char* linkStart = strchr(bodyPtr, '(');
        char* linkClose = strchr(bodyPtr, ')');

        if (linkEnd && linkStart && linkClose && linkStart > linkEnd && linkClose > linkStart) {
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
void documentPrint(Document* doc) {
    if (!doc) return;

    printf("\n=== Document %d ===\n", doc->id);
    printf("Titol: %s\n", doc->title);
    printf("Relevancia: %.2f\n", doc->relevancia);
    printf("\nCos:\n%s\n", doc->body);

    if (doc->links) {
        printf("\nLinks:\n");
        Link* current = doc->links;
        while (current) {
            printf("(%d) ", current->id);
            if (current->next) printf("-> ");
            current = current->next;
        }
        printf("\n");
    }
    printf("\n");
}
void documentFree(Document* doc) {
    if (!doc) return;
    free(doc->title);
    free(doc->body);
    linkFree(doc->links);
	free(doc);
}


DocumentsList* documentsListCreate() {
    DocumentsList* list = malloc(sizeof(DocumentsList));
    if (!list) return NULL;
    list->head = NULL;
    list->size = 0;
    return list;
}
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
    list->size++;
}
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
void documentsListPrint(DocumentsList* list) {
    if (!list || !list->head) {
        printf("No documents found.\n");
        return;
    }

    printf("\nAvailable documents:\n");
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
