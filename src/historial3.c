#include "historial3.h"
#include <stdio.h>
#include <stdlib.h>

//Crea y inicializa historial consultas vacío
//Reserva memoria para estructura y devuelve un puntero a ella
QueryHistory* queryHistoryCreate() {
    QueryHistory* history = malloc(sizeof(QueryHistory));
    if (!history){
       return NULL; //Si falla al asignar memoria, devuelve NULL
    }

    history->front = NULL;
    history->rear = NULL;
    history->count = 0;

    return history;
}

//Añade una consulta al historial, (max 3)
//Inserta la consulta al final de la lista enlazada y elimina la más antigua si es necesario (cola)
void queryHistoryEnqueue(QueryHistory* history, char* queryString) {
    if (!history || !queryString) return;

    //Crea nuevo nodo con la consulta proporcionada
    QueryHistoryNode* node = malloc(sizeof(QueryHistoryNode));
    if (!node) return;

    node->queryString = malloc(strlen(queryString) + 1);
    strcpy(node->queryString, queryString);
    node->next = NULL;

    //Si cola vacía, nuevo nodo se convierte en el primer y último elemento
    if (history->rear == NULL) {
        history->front = history->rear = node;
    } else {
        history->rear->next = node;
        history->rear = node;
    }

    history->count++;

    //últimas 3 consultas eliminando la más antigua
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

//Función imprime las consultas almacenadas en el historial
//Recorre lista enlazada y enseña cada consulta en orden
void queryHistoryPrint(QueryHistory* history) {
    if (!history || history->count == 0) {
        printf("No hay búsquedas recientes\n");
        return;
    }

    printf("Últimas búsquedas (3 ult):\n");
    QueryHistoryNode* current = history->front;
    int index = 1;
    while (current) {
        printf("  %d. \"%s\"\n", index, current->queryString);
        current = current->next;
        index++;
    }
    printf("\n");
}

//Función libera memoria utilizada por el historial de consultas, recorre linked list y libera cada nodo
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