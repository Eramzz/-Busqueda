#ifndef GRAPH_H
#define GRAPH_H

#include "document.h"

//Estructura nodo del grafo (doc)
typedef struct GraphNode {
    int documentId;  //ID doc
    int indegree;    //Cuántos documentos apuntan a este, grado
    float pageRank;  //PageRank para medir su importancia
    struct GraphEdge* edges;  //Lista de aristas
    struct GraphNode* next;   //Puntero sig nodo
} GraphNode;

//Estructura arista del grafo,enlace dos documentos
typedef struct GraphEdge {
    int destinationId;  //ID doc al que se apunta
    struct GraphEdge* next; //puntero a siguiente arista del mismo nodo origen
} GraphEdge;

//Estructura grafo
typedef struct DocumentGraph {
    GraphNode* nodes; //Lista nodos
    int nodeCount; //num nodos
    int edgeCount; //num aristas
} DocumentGraph;

DocumentGraph* documentGraphCreate(); //Función crea e inicializa un grafo vacío
GraphNode* documentGraphFindNode(DocumentGraph* graph, int documentId); //busca nodo en el grafo por su ID de documento
GraphNode* documentGraphAddNode(DocumentGraph* graph, int documentId); //añade un nodo al grafo si no existe ya
void documentGraphAddEdge(DocumentGraph* graph, int sourceId, int destinationId); //añade arista desde un documento origen hacia uno destino
void documentGraphBuildFromDocuments(DocumentGraph* graph, DocumentsList* documents); //construye el grafo completo a partir de una lista de documentos con enlaces
float documentGraphGetIndegree(DocumentGraph* graph, int documentId); //devuelve el grado de entrada de un documento con cuantos esta enlazado
void documentGraphCalculatePageRank(DocumentGraph* graph, float dampingFactor, int maxIterations, float tolerance); //calcula los valores PageRank todos los doc (nodos) del grafo
void documentGraphPrint(DocumentGraph* graph); //imprime grafoen general (nodos, aristas, top documentos)
void documentGraphFree(DocumentGraph* graph); //libera toda la memoria del grafo (nodos y aristas)
void documentCalculateRelevance(Document* doc, DocumentGraph* graph); //calcula la relevancia de un documento usando PageRank o grado
DocumentsList* documentsListSortedDescending(DocumentsList* list); //ordena la lista de documentos por relevancia de mayor a menor
int documentGraphSerializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename); //guarda relevancia en un archivo para no tener que recalcularlos luego
int documentGraphDeserializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename); //carga valores de relevancia previamente guardados desde un archivo

#endif