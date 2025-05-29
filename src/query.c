#include "query.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Crea una nova query
QueryItem* queryItemCreate(char* word, int isExclude) {
    QueryItem* item = malloc(sizeof(QueryItem));
    if (!item) return NULL;

    item->word = malloc(strlen(word) + 1);
    strcpy(item->word, word);
    item->isExclude = isExclude;
    item->next = NULL;

    return item;
}

void queryAppendItem(Query* query, QueryItem* item) {
    if (!query || !item) return;

    if (query->head == NULL) {
        query->head = item;
    } else {
        QueryItem* current = query->head;
        while (current->next) {
            current = current->next;
        }
        current->next = item;
    }
    query->itemCount++;
}

//A minuscula i sense puntuació
char* normalitzar(char* word) {
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

// Initialitza amb input
Query* queryInit(char* input) {
    if (!input) return NULL;

    Query* query = malloc(sizeof(Query));
    if (!query) return NULL;

    query->head = NULL;
    query->itemCount = 0;

    // Create a copy of the query string to tokenize
    char* queryStr = malloc(strlen(input) + 1);
    strcpy(queryStr, input);

    char* token = strtok(queryStr, " \t\n");
    while (token != NULL) {
        int isExclude = 0;
        char* word = token;

        // Check for exclusion prefix (-)
        if (token[0] == '-' && strlen(token) > 1) {
            isExclude = 1;
            word = token + 1; // Skip the '-' character
        }

        // Clean the word
        char* cleanedWord = normalitzar(word);
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

void queryPrint(Query* query) {
    if (!query) return;

    printf("Query: ");
    QueryItem* current = query->head;
    while (current) {
        if (current->isExclude) {
            printf("-%s ", current->word);
        } else {
            printf("%s ", current->word);
        }
        current = current->next;
    }
    printf("(%d items)\n", query->itemCount);
}

void queryFree(Query* query) {
    if (!query) return;

    QueryItem* current = query->head;
    while (current) {
        QueryItem* temp = current;
        current = current->next;
        free(temp->word);
        free(temp);
    }
    free(query);
}

// Crea una cua d'histiria
QueryHistory* queryHistoryCreate() {
    QueryHistory* history = malloc(sizeof(QueryHistory));
    if (!history) return NULL;

    history->front = NULL;
    history->rear = NULL;
    history->count = 0;

    return history;
}

// Afageix query al historial
void queryHistoryEnqueue(QueryHistory* history, char* input) {
    if (!history || !input) return;

    QueryHistoryNode* node = malloc(sizeof(QueryHistoryNode));
    if (!node) return;

    node->input = malloc(strlen(input) + 1);
    strcpy(node->input, input);
    node->next = NULL;

    if (history->rear == NULL) {
        history->front = history->rear = node;
    } else {
        history->rear->next = node;
        history->rear = node;
    }

    history->count++;

    // Es queda amb les ultimes 3
    while (history->count > 3) {
        QueryHistoryNode* temp = history->front;
        history->front = history->front->next;
        if (history->front == NULL) {
            history->rear = NULL;
        }
        free(temp->input);
        free(temp);
        history->count--;
    }
}

void queryHistoryPrint(QueryHistory* history) {
    if (!history || history->count == 0) {
        printf("No recent searches.\n");
        return;
    }

    printf("Recent searches:\n");
    QueryHistoryNode* current = history->front;
    int index = 1;
    while (current) {
        printf("  %d. \"%s\"\n", index, current->input);
        current = current->next;
        index++;
    }
    printf("\n");
}

void queryHistoryFree(QueryHistory* history) {
    if (!history) return;

    QueryHistoryNode* current = history->front;
    while (current) {
        QueryHistoryNode* temp = current;
        current = current->next;
        free(temp->input);
        free(temp);
    }
    free(history);
}

//Mira si en el document hi ha la paraula
int documentContainsWord(Document* doc, char* word) {
    if (!doc || !word) return 0;

    // mira titul
    char* cleanedTitle = normalitzar(doc->title);
    if (cleanedTitle && strstr(cleanedTitle, word)) {
        free(cleanedTitle);
        return 1;
    }
    free(cleanedTitle);

    // mira el cos
    char* cleanedBody = normalitzar(doc->body);
    if (cleanedBody && strstr(cleanedBody, word)) {
        free(cleanedBody);
        return 1;
    }
    free(cleanedBody);

    return 0;
}

// Crea una copia
DocumentsList* documentsListCopy(DocumentsList* source) {
    if (!source) return NULL;

    DocumentsList* copy = documentsListCreate();
    if (!copy) return NULL;

    Document* current = source->head;
    while (current) {
        // Create a shallow copy (references same documents)
        Document* docCopy = malloc(sizeof(Document));
        *docCopy = *current;
        docCopy->next = NULL;

        documentsListAppend(copy, docCopy);
        current = current->next;
    }

    return copy;
}

// Linear search
DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query) {
    if (!documents || !query) return NULL;

    DocumentsList* results = documentsListCreate();
    if (!results) return NULL;

    Document* current = documents->head;

    while (current) {
        int matches = 1; // Assume document matches until proven otherwise

        // Check all query items
        QueryItem* queryItem = query->head;
        while (queryItem && matches) {
            int contains = documentContainsWord(current, queryItem->word);

            if (queryItem->isExclude) {
                // For exclude items, document should NOT contain the word
                if (contains) {
                    matches = 0;
                }
            } else {
                // For include items, document MUST contain the word
                if (!contains) {
                    matches = 0;
                }
            }

            queryItem = queryItem->next;
        }

        if (matches) {
            // Create a shallow copy for results
            Document* docCopy = malloc(sizeof(Document));
            *docCopy = *current;
            docCopy->next = NULL;

            documentsListAppend(results, docCopy);
        }

        current = current->next;
    }

    return results;
}