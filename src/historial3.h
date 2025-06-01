#ifndef HISTORIAL3_H
#define HISTORIAL3_H

//Estructura nodo de la lista enlazada que representa una consulta almacenada en el historial
typedef struct QueryHistoryNode {
    char* queryString;  //string consulta (palabra)
    struct QueryHistoryNode* next;  //puntero a sig nodo
} QueryHistoryNode;

//Estructura del historial de consultas
typedef struct QueryHistory {
    QueryHistoryNode* front;  //primer nodo del historial (head)
    QueryHistoryNode* rear;  //ultimo nodo del historial (tail)
    int count;  //num consultas almacenadas
} QueryHistory;

QueryHistory* queryHistoryCreate(); //inicializa cola para el historial de consultas
void queryHistoryEnqueue(QueryHistory* history, char* queryString); //Añade nueva consulta a la cola
void queryHistoryPrint(QueryHistory* history);//Imprime todas las consultas por pantalla
void queryHistoryFree(QueryHistory* history); //Libera la memoria historial de consultas

#endif//HISTORIAL3_H
