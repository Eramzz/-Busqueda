#include "historial3.h"

// Crea un historial de consultas
QueryHistory* queryHistoryCreate() {
    QueryHistory* history = malloc(sizeof(QueryHistory));
    if (!history) return NULL;

    history->front = NULL;
    history->rear = NULL;
    history->count = 0;

    return history;
}

// Añade una consulta al historial (cola)
void queryHistoryEnqueue(QueryHistory* history, char* queryString) {
    if (!history || !queryString) return;

    QueryHistoryNode* node = malloc(sizeof(QueryHistoryNode));
    if (!node) return;

    node->queryString = malloc(strlen(queryString) + 1);
    strcpy(node->queryString, queryString);
    node->next = NULL;

    if (history->rear == NULL) {
        history->front = history->rear = node;
    } else {
        history->rear->next = node;
        history->rear = node;
    }

    history->count++;

    // Mantiene solo las últimas 3 consultas
    while (history->count > 3) {
        QueryHistoryNode* temp = history->front;
        history->front = history->front->next;
        if (history->front == NULL) {
            history->rear = NULL;
        }
        free(temp->queryString);
        free(temp);
        history->count--;
    }
}

// Imprime el historial de consultas
void queryHistoryPrint(QueryHistory* history) {
    if (!history || history->count == 0) {
        printf("No hay búsquedas recientes.\n");
        return;
    }

    printf("Búsquedas recientes:\n");
    QueryHistoryNode* current = history->front;
    int index = 1;
    while (current) {
        printf("  %d. \"%s\"\n", index, current->queryString);
        current = current->next;
        index++;
    }
    printf("\n");
}

// Libera la memoria del historial
void queryHistoryFree(QueryHistory* history) {
    if (!history) return;

    QueryHistoryNode* current = history->front;
    while (current) {
        QueryHistoryNode* temp = current;
        current = current->next;
        free(temp->queryString);
        free(temp);
    }
    free(history);
}