#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//crea un nuevo grafo de documentos
DocumentGraph* documentGraphCreate() {
    DocumentGraph* graph = malloc(sizeof(DocumentGraph)); //guarda memoria para la estructura del grafo
    if (!graph){
        return NULL; //si falla, retorna null
    }

    graph->nodes = NULL; //lista de nodos vacía
    graph->nodeCount = 0; //num nodos = 0
    graph->edgeCount = 0; //num aristas = 0

    return graph; //devuelve el grafo
}

//busca un nodo en el grafo por id de documento
GraphNode* documentGraphFindNode(DocumentGraph* graph, int documentId) {
    if (!graph){
        return NULL; //si el grafo no existe devuelve null
    }

    GraphNode* current = graph->nodes; //comienza desde el primer nodo del grafo
    while (current) {
        if (current->documentId == documentId) { //si el ID del nodo coincide con el buscado
            return current; //devuelve el nodo encontrado
        }
        current = current->next; //pasa al siguiente nodo
    }
    return NULL; //si no se encuentra ningún nodo con ese ID, devuelve null
}

// añade un nodo al grafo
GraphNode* documentGraphAddNode(DocumentGraph* graph, int documentId) {
    if (!graph){
        return NULL; //si el grafo no existe devuelve null
    }

    //mira si el nodo ya existe
    GraphNode* existing = documentGraphFindNode(graph, documentId);
    if (existing) return existing; //si ya existe, retorna el nodo existente

    // crea un nuevo nodo
    GraphNode* node = malloc(sizeof(GraphNode)); //guarda memoria para el nuevo nodo
    if (!node){
        return NULL; //si falla la memoria, retorna null
    }

    node->documentId = documentId; //asigna el id del documento al nodo
    node->indegree = 0; //inicializa el grado de entrada en 0
    node->pageRank = 0.0; //inicializa el pagerank en 0
    node->edges = NULL; //no tiene aristas todavía
    node->next = graph->nodes; //lo enlaza al inicio de la lista de nodos del grafo

    graph->nodes = node; //lo establece como el primer nodo del grafo
    graph->nodeCount++; //incrementa el contador de nodos del grafo

    return node; //devuelve el nodo creado
}

//añade una arista al grafo
void documentGraphAddEdge(DocumentGraph* graph, int sourceId, int destinationId) {
    if (!graph){
        return; //si el grafo no existe, termina la función
    }

    //asegura que ambos nodos existan en el grafo
    GraphNode* sourceNode = documentGraphAddNode(graph, sourceId); //obtiene o crea el nodo fuente
    GraphNode* destinationNode = documentGraphAddNode(graph, destinationId); //obtiene o crea el nodo destino

    if (!sourceNode || !destinationNode){
        return; //si alguno no se pudo obtener o crear, termina la función
    }

    //mira si la arista ya existe entre los dos nodos
    struct GraphEdge* currentEdge = sourceNode->edges;
    while (currentEdge) {
        if (currentEdge->destinationId == destinationId) {
            return; //la arista ya existe, no se añade de nuevo
        }
        currentEdge = currentEdge->next;
    }

    //crea una nueva arista desde el nodo fuente hacia el nodo destino
    struct GraphEdge* edge = malloc(sizeof(struct GraphEdge)); //reserva memoria para la arista
    if (!edge){
        return; //si falla termina
    }

    edge->destinationId = destinationId; //asigna el ID del nodo destino a la arista
    edge->next = sourceNode->edges; //enlaza la nueva arista al principio de la lista de aristas del nodo fuente
    sourceNode->edges = edge; //actualiza la lista de aristas del nodo fuente

    //incrementa el grado de entrada del nodo destino
    destinationNode->indegree++; //el nodo destino ahora tiene una arista más apuntando a él
    graph->edgeCount++; //incrementa el contador total de aristas en el grafo
}

//construye el grafo a partir de una lista de documentos
void documentGraphBuildFromDocuments(DocumentGraph* graph, DocumentsList* documents) {
    if (!graph || !documents){
        return; //si el grafo o la lista de documentos no existen, termina la función
    }
    printf("se esta construyendo el grafo de documentos.........\n");

    Document* current = documents->head; //comienza desde el primer documento en la lista
    int processedNodes = 0; //contador de nodos procesados

    //1: crea todos los nodos del grafo a partir de los documentos
    while (current) {
        documentGraphAddNode(graph, current->id); //añade un nodo al grafo por cada documento
        processedNodes++; //incrementa el contador de nodos
        current = current->next; //avanza al siguiente documento
    }
    printf("hay %d nodos creados\n", graph->nodeCount);

    //2: crea aristas a partir de los enlaces de cada documento
    current = documents->head; //vuelve al inicio de la lista de documentos
    int processedEdges = 0; //contador de aristas procesadas

    while (current) {
        Link* link = current->links; //obtiene la lista de enlaces del documento actual
        while (link) {
            documentGraphAddEdge(graph, current->id, link->id); //añade una arista desde el documento actual al documento enlazado
            processedEdges++; //suma 1 al contador de aristas
            link = link->next; //pasa al siguiente enlace del documento
        }
        current = current->next; //pasa al siguiente documento
    }
    printf("hay %d aristas creadas\n", graph->edgeCount);

    printf("ya se ha acabado de construir el grafo de documentos\n");
}

//obtiene el grado de entrada (indegree) de un nodo del grafo
float documentGraphGetIndegree(DocumentGraph* graph, int documentId) {
    if (!graph){
        return 0.0; //si el grafo no existe, devuelve 0.0
    }

    GraphNode* node = documentGraphFindNode(graph, documentId); //busca el nodo correspondiente con el ID del documento
    if (!node){
        return 0.0; //si no se encuentra el nodo, devuelve 0.0
    }
    return (float)node->indegree; //devuelve el grado de entrada del nodo como float
}


//libera la memoria del grafo
void documentGraphFree(DocumentGraph* graph) {
    if (!graph){
        return; //si el grafo no existe, termina
    }

    GraphNode* current = graph->nodes; //empieza desde el primer nodo del grafo
    while (current) {
        GraphNode* nodeToFree = current; //guarda el nodo actual para liberarlo más tarde
        current = current->next; //pasa al siguiente nodo

        //libera todas las aristas del nodo
        struct GraphEdge* edge = nodeToFree->edges; //punetro que apunta a la primera arista del nodo
        while (edge) {
            struct GraphEdge* edgeToFree = edge; //guarda la arista actual
            edge = edge->next; //pasa a la siguiente arista
            free(edgeToFree); //libera la arista actual
        }
        free(nodeToFree); //libera memoria del nodo actual
    }
    free(graph); //libera memoria grafo
}

//calcula pagerank para todos los nodos
void documentGraphCalculatePageRank(DocumentGraph* graph, float dampingFactor, int maxIterations, float tolerance) {
    if (!graph || graph->nodeCount == 0){
        return; //si el grafo es nulo o no tiene nodos, termina, ya que no puede calcular el pagerank
    }

    printf("calculando pagerank(factor=%.2f, iteraciones=%d, tolerancia=%.6f)...\n",
           dampingFactor, maxIterations, tolerance);

    //inicializa valores de pagerank iguales para todos los nodos
    float initialRank = 1.0 / graph->nodeCount;
    GraphNode* current = graph->nodes;
    while (current) {
        current->pageRank = initialRank; //asigna pagerank inicial
        current = current->next;
    }

    //empieza el cálculo iterativo de pagerank
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        float* newRanks = calloc(graph->nodeCount, sizeof(float)); //crea un arreglo para almacenar nuevos pageranks
        GraphNode** nodeArray = malloc(graph->nodeCount * sizeof(GraphNode*)); //arreglo para acceso rápido a nodos

        //llena el arreglo de nodos y asigna el valor base para el pagerank nuevo
        current = graph->nodes;
        int index = 0;
        while (current) {
            nodeArray[index] = current;
            newRanks[index] = (1.0 - dampingFactor) / graph->nodeCount; //parte constante del cálculo de pagerank
            index++;
            current = current->next;
        }

        //calcula los nuevos valores de pagerank para cada nodo
        for (int i = 0; i < graph->nodeCount; i++) {
            GraphNode* node = nodeArray[i];
            struct GraphEdge* edge = node->edges;

            //cuenta cuántas aristas salientes tiene el nodo actual
            int outDegree = 0;
            while (edge) {
                outDegree++;
                edge = edge->next;
            }

            if (outDegree > 0) {
                edge = node->edges;
                //distribuye el pagerank entre los nodos destino de las aristas
                while (edge) {
                    //busca el índice del nodo destino en el arreglo
                    for (int j = 0; j < graph->nodeCount; j++) {
                        if (nodeArray[j]->documentId == edge->destinationId) {
                            newRanks[j] += dampingFactor * (node->pageRank / outDegree); //actualiza el pagerank del nodo destino
                            break;
                        }
                    }
                    edge = edge->next;
                }
            }
        }

        //comprueba la convergencia comparando los valores nuevos con los anteriores
        float maxDiff = 0.0;
        for (int i = 0; i < graph->nodeCount; i++) {
            float diff = fabs(newRanks[i] - nodeArray[i]->pageRank);
            if (diff > maxDiff) {
                maxDiff = diff;
            }
            nodeArray[i]->pageRank = newRanks[i]; //actualiza el pagerank con nuevo valor
        }

        free(newRanks); //libera la memoria de los nuevos pageranks temporales
        free(nodeArray); //libera la memoria del array de nodos

        if (maxDiff < tolerance) { //si la diferencia máxima es menor que la tolerancia, se considera convergente
            printf("el pagerank ha convergido tras hacer %d iteraciones (diff=%.6f).\n", iteration + 1, maxDiff);
            return;
        }

        if ((iteration + 1) % 10 == 0) {
            printf("iteración %d, diff máxima: %.6f\n", iteration + 1, maxDiff);
        }
    }
    printf("pagerank completó %d iteraciones (sin convergencia).\n", maxIterations);
}

//calcula la relevancia de un documento usando el grafo
void documentCalculateRelevance(Document* doc, DocumentGraph* graph) {
    if (!doc || !graph){
        return; //si el documento o el grafo no existen, termina
    }

    GraphNode* node = documentGraphFindNode(graph, doc->id); //busca el nodo correspondiente al documento en el grafo
    if (node) {
        //usa pagerank si está disponible, si no, grado de entrada
        if (node->pageRank > 0.0) {
            doc->relevance = node->pageRank * 1000.0; //multiplica pagerank por 1000 para visualizarlo mejor
        } else {
            doc->relevance = (float)node->indegree; //si no hay pagerank, usa el número de enlaces entrantes
        }
    } else {
        doc->relevance = 0.0; //si el nodo no está en el grafo, la relevancia es 0
    }
}

//ordena la lista de documentos por relevancia (descendente)
DocumentsList* documentsListSortedDescending(DocumentsList* list) {
    if (!list || list->count <= 1) return list; //si la lista no existe o tiene 1 o menos elementos, no se necesita ordenar
    int swapped;
    do {
        swapped = 0; //indica si se hicieron intercambios en esta pasada
        Document* current = list->head; //empieza desde el principio de la lista

        while (current && current->next) { //mientras haya un siguiente nodo
            if (current->relevance < current->next->relevance) { //si el actual tiene menor relevancia que el siguiente
                //intercambia datos
                int tempId = current->id;
                char* tempTitle = current->title;
                char* tempBody = current->body;
                Link* tempLinks = current->links;
                float tempRelevance = current->relevance;

                current->id = current->next->id;
                current->title = current->next->title;
                current->body = current->next->body;
                current->links = current->next->links;
                current->relevance = current->next->relevance;

                current->next->id = tempId;
                current->next->title = tempTitle;
                current->next->body = tempBody;
                current->next->links = tempLinks;
                current->next->relevance = tempRelevance;

                swapped = 1; // se hizo al menos un intercambio
            }
            current = current->next; //pasa al siguiente nodo
        }
    } while (swapped); //repite mientras se hagan intercambios

    return list; //devuelve la lista ordenada
}

//imprime estadísticas del grafo
void documentGraphPrint(DocumentGraph* graph) {
    if (!graph){
        return; // si el grafo no existe, termina
    }

    printf("Grafo de documentos:\n");
    printf("nodos: %d\n", graph->nodeCount);
    printf("aristas: %d\n", graph->edgeCount);

    printf("\ntop 10 documentos por grado de entrada:\n");

    //ordenación simple para visualización (arreglo de punteros a nodos)
    GraphNode* nodes[10] = {NULL}; //arreglo para almacenar los 10 nodos con mayor indegree
    GraphNode* current = graph->nodes; //empieza desde el primer nodo de la lista

    while (current) { //recorre todos los nodos
        for (int i = 0; i < 10; i++) { //recorre el arreglo de top 10
            //si la posición está vacía o el nodo actual tiene mayor indegree que el nodo en la posición i
            if (nodes[i] == NULL || current->indegree > nodes[i]->indegree) {
                //desplaza hacia abajo los nodos para hacer espacio
                for (int j = 9; j > i; j--) {
                    nodes[j] = nodes[j-1];
                }
                nodes[i] = current; //inserta el nodo actual en la posición i
                break; //termina el for porque ya insertó el nodo
            }
        }
        current = current->next; //pasa al siguiente nodo en la lista
    }

    //imprime el top 10 de nodos por indegree
    for (int i = 0; i < 10 && nodes[i] != NULL; i++) {
        printf("  %d. documento %d: grado=%d, pagerank=%.4f\n",
               i+1, nodes[i]->documentId, nodes[i]->indegree, nodes[i]->pageRank);
    }
    printf("\n");
}

//serializa las puntuaciones de relevancia a un archivo
int documentGraphSerializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename) {
    if (!graph || !documents || !filename){
        return 0; //mira que los parametros que le han entrado no sean nulos, retorna 0 (error)
    }
    FILE* file = fopen(filename, "w"); // abre el archivo para escritura
    if (!file){
        return 0; //si no se puede abrir el archivo, devuelve 0 (error)
    }

    Document* current = documents->head; //empieza desde el primer documento de la lista
    while (current) { //recorre todos los documentos
        GraphNode* node = documentGraphFindNode(graph, current->id); //busca el nodo correspondiente en el grafo
        if (node) { //si el nodo existe
            //escribe en el archivo: id del documento, grado de entrada y pagerank con 6 decimales
            fprintf(file, "%d %.6f %.6f\n", node->documentId, (float)node->indegree, node->pageRank);
        }
        current = current->next; //pasa al siguiente documento
    }

    fclose(file); //cierra archivo
    return 1; // devuelve 1 quiere decir que ha salido bien
}

//deserializa las puntuaciones de relevancia desde un archivo
int documentGraphDeserializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename) {
    if (!graph || !documents || !filename){
       return 0; //mira si alguno de los parámetros que le entran es nulo, entonces devuelve 0 (error)
    }

    FILE* file = fopen(filename, "r"); //abre el archivo modo lectura
    if (!file){
        return 0; //si no se puede abrir el archivo, retorna 0 (error)
    }

    int documentId;  //inicializavariable para leer el id del documento
    float indegree, pageRank; //inicializa variables para leer el grado de entrada y pagerank

    //lee hasta que no haya más líneas con tres valores (id, indegree, pagerank)
    while (fscanf(file, "%d %f %f", &documentId, &indegree, &pageRank) == 3) {
        GraphNode* node = documentGraphFindNode(graph, documentId); //busca el nodo correspondiente en el grafo
        if (node) { //si el nodo existe
            node->indegree = (int)indegree; //asigna el grado de entrada (convierte float a int)
            node->pageRank = pageRank;   //asigna el pagerank
        }
    }

    fclose(file); //cierra el archivo
    return 1; //devuelve1, ha salido bien
}