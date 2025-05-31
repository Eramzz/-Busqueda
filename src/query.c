#include "query.h"
#include <ctype.h>
#include <string.h>

char* cleanWord(char* word) {
    if (!word) return NULL;

    int len = strlen(word);
    char* cleaned = malloc(len + 1);
    int j = 0;

    for (int i = 0; i < len; i++) {
        if (isalnum(word[i])) {
            cleaned[j++] = tolower(word[i]);
        }
    }
    cleaned[j] = '\0';

    return cleaned;
}

// Crea un nuevo ítem de consulta
QueryItem* queryItemCreate(char* word, int isExclude) {
    QueryItem* item = malloc(sizeof(QueryItem));
    if (!item) return NULL;

    item->word = malloc(strlen(word) + 1);
    strcpy(item->word, word);
    item->isExclude = isExclude;
    item->next = NULL;

    return item;
}

// Añade un ítem a la consulta
void queryAppendItem(Query* query, QueryItem* item) {
    if (!query || !item) return;

    if (query->items == NULL) {
        query->items = item;
    } else {
        QueryItem* current = query->items;
        while (current->next) {
            current = current->next;
        }
        current->next = item;
    }
    query->itemCount++;
}

// Inicializa una consulta desde un string
Query* queryInit(char* queryString) {
    if (!queryString) return NULL;

    Query* query = malloc(sizeof(Query));
    if (!query) return NULL;

    query->items = NULL;
    query->itemCount = 0;

    // Copia el string para tokenizarlo
    char* queryStr = malloc(strlen(queryString) + 1);
    strcpy(queryStr, queryString);

    char* token = strtok(queryStr, " \t\n");
    while (token != NULL) {
        int isExclude = 0;
        char* word = token;

        // Verifica si es un término de exclusión (-)
        if (token[0] == '-' && strlen(token) > 1) {
            isExclude = 1;
            word = token + 1; // Omite el carácter '-'
        }

        // Limpia la palabra
        char* cleanedWord = cleanWord(word);
        if (cleanedWord && strlen(cleanedWord) > 0) {
            QueryItem* item = queryItemCreate(cleanedWord, isExclude);
            if (item) {
                queryAppendItem(query, item);
            }
        }
        free(cleanedWord);

        token = strtok(NULL, " \t\n");
    }

    free(queryStr);
    return query;
}

// Imprime la consulta
void queryPrint(Query* query) {
    if (!query) return;

    printf("Consulta: ");
    QueryItem* current = query->items;
    while (current) {
        if (current->isExclude) {
            printf("-%s ", current->word);
        } else {
            printf("%s ", current->word);
        }
        current = current->next;
    }
    printf("(%d ítems)\n", query->itemCount);
}

// Libera la memoria de la consulta
void queryFree(Query* query) {
    if (!query) return;

    QueryItem* current = query->items;
    while (current) {
        QueryItem* temp = current;
        current = current->next;
        free(temp->word);
        free(temp);
    }
    free(query);
}