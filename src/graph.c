#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Crea un nuevo grafo de documentos
DocumentGraph* documentGraphCreate() {
    DocumentGraph* graph = malloc(sizeof(DocumentGraph));
    if (!graph) return NULL;

    graph->nodes = NULL;
    graph->nodeCount = 0;
    graph->edgeCount = 0;

    return graph;
}

// Busca un nodo en el grafo por ID de documento
GraphNode* documentGraphFindNode(DocumentGraph* graph, int documentId) {
    if (!graph) return NULL;

    GraphNode* current = graph->nodes;
    while (current) {
        if (current->documentId == documentId) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Añade un nodo al grafo
GraphNode* documentGraphAddNode(DocumentGraph* graph, int documentId) {
    if (!graph) return NULL;

    // Verifica si el nodo ya existe
    GraphNode* existing = documentGraphFindNode(graph, documentId);
    if (existing) return existing;

    // Crea un nuevo nodo
    GraphNode* node = malloc(sizeof(GraphNode));
    if (!node) return NULL;

    node->documentId = documentId;
    node->indegree = 0;
    node->pageRank = 0.0;
    node->edges = NULL;
    node->next = graph->nodes;

    graph->nodes = node;
    graph->nodeCount++;

    return node;
}

// Añade una arista al grafo
void documentGraphAddEdge(DocumentGraph* graph, int sourceId, int destinationId) {
    if (!graph) return;

    // Asegura que ambos nodos existan
    GraphNode* sourceNode = documentGraphAddNode(graph, sourceId);
    GraphNode* destinationNode = documentGraphAddNode(graph, destinationId);

    if (!sourceNode || !destinationNode) return;

    // Verifica si la arista ya existe
    struct GraphEdge* currentEdge = sourceNode->edges;
    while (currentEdge) {
        if (currentEdge->destinationId == destinationId) {
            return; // La arista ya existe
        }
        currentEdge = currentEdge->next;
    }

    // Crea una nueva arista
    struct GraphEdge* edge = malloc(sizeof(struct GraphEdge));
    if (!edge) return;

    edge->destinationId = destinationId;
    edge->next = sourceNode->edges;
    sourceNode->edges = edge;

    // Incrementa el grado de entrada del nodo destino
    destinationNode->indegree++;
    graph->edgeCount++;
}

// Construye el grafo a partir de una lista de documentos
void documentGraphBuildFromDocuments(DocumentGraph* graph, DocumentsList* documents) {
    if (!graph || !documents) return;

    printf("Construyendo grafo de documentos...\n");

    Document* current = documents->head;
    int processedNodes = 0;

    // Primer paso: crea todos los nodos
    while (current) {
        documentGraphAddNode(graph, current->id);
        processedNodes++;
        current = current->next;
    }

    printf("Creados %d nodos.\n", graph->nodeCount);

    // Segundo paso: crea aristas desde los enlaces
    current = documents->head;
    int processedEdges = 0;

    while (current) {
        Link* link = current->links;
        while (link) {
            documentGraphAddEdge(graph, current->id, link->id);
            processedEdges++;
            link = link->next;
        }
        current = current->next;
    }

    printf("Creadas %d aristas.\n", graph->edgeCount);
    printf("Grafo de documentos construido con éxito.\n");
}

float documentGraphGetIndegree(DocumentGraph* graph, int documentId) {
    if (!graph) return 0.0;

    GraphNode* node = documentGraphFindNode(graph, documentId);
    if (!node) return 0.0;

    return (float)node->indegree;
}


// Libera la memoria del grafo
void documentGraphFree(DocumentGraph* graph) {
    if (!graph) return;

    GraphNode* current = graph->nodes;
    while (current) {
        GraphNode* nodeToFree = current;
        current = current->next;

        // Libera aristas
        struct GraphEdge* edge = nodeToFree->edges;
        while (edge) {
            struct GraphEdge* edgeToFree = edge;
            edge = edge->next;
            free(edgeToFree);
        }
        free(nodeToFree);
    }
    free(graph);
}

// Calcula PageRank para todos los nodos
void documentGraphCalculatePageRank(DocumentGraph* graph, float dampingFactor, int maxIterations, float tolerance) {
    if (!graph || graph->nodeCount == 0) return;

    printf("Calculando PageRank (factor=%.2f, iteraciones=%d, tolerancia=%.6f)...\n",
           dampingFactor, maxIterations, tolerance);

    // Inicializa valores de PageRank
    float initialRank = 1.0 / graph->nodeCount;
    GraphNode* current = graph->nodes;
    while (current) {
        current->pageRank = initialRank;
        current = current->next;
    }

    // Cálculo iterativo
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        float* newRanks = calloc(graph->nodeCount, sizeof(float));
        GraphNode** nodeArray = malloc(graph->nodeCount * sizeof(GraphNode*));

        // Prepara un array de nodos para acceso rápido
        current = graph->nodes;
        int index = 0;
        while (current) {
            nodeArray[index] = current;
            newRanks[index] = (1.0 - dampingFactor) / graph->nodeCount;
            index++;
            current = current->next;
        }

        // Calcula nuevos valores de PageRank
        for (int i = 0; i < graph->nodeCount; i++) {
            GraphNode* node = nodeArray[i];
            struct GraphEdge* edge = node->edges;

            // Cuenta aristas salientes
            int outDegree = 0;
            while (edge) {
                outDegree++;
                edge = edge->next;
            }

            if (outDegree > 0) {
                edge = node->edges;
                while (edge) {
                    // Encuentra el nodo destino
                    for (int j = 0; j < graph->nodeCount; j++) {
                        if (nodeArray[j]->documentId == edge->destinationId) {
                            newRanks[j] += dampingFactor * (node->pageRank / outDegree);
                            break;
                        }
                    }
                    edge = edge->next;
                }
            }
        }

        // Verifica convergencia
        float maxDiff = 0.0;
        for (int i = 0; i < graph->nodeCount; i++) {
            float diff = fabs(newRanks[i] - nodeArray[i]->pageRank);
            if (diff > maxDiff) {
                maxDiff = diff;
            }
            nodeArray[i]->pageRank = newRanks[i];
        }

        free(newRanks);
        free(nodeArray);

        if (maxDiff < tolerance) {
            printf("PageRank convergió después de %d iteraciones (diff=%.6f).\n", iteration + 1, maxDiff);
            return;
        }

        if ((iteration + 1) % 10 == 0) {
            printf("Iteración %d, diff máxima: %.6f\n", iteration + 1, maxDiff);
        }
    }
    printf("PageRank completó %d iteraciones (sin convergencia).\n", maxIterations);
}

// Calcula la relevancia de un documento usando el grafo
void documentCalculateRelevance(Document* doc, DocumentGraph* graph) {
    if (!doc || !graph) return;

    GraphNode* node = documentGraphFindNode(graph, doc->id);
    if (node) {
        // Usa PageRank si está disponible, sino usa el grado de entrada
        if (node->pageRank > 0.0) {
            doc->relevance = node->pageRank * 1000.0; // Escala para visualización
        } else {
            doc->relevance = (float)node->indegree;
        }
    } else {
        doc->relevance = 0.0;
    }
}

// Ordena la lista de documentos por relevancia (descendente)
DocumentsList* documentsListSortedDescending(DocumentsList* list) {
    if (!list || list->count <= 1) return list;

    // Ordenación burbuja (suficiente para conjuntos pequeños)
    int swapped;
    do {
        swapped = 0;
        Document* current = list->head;

        while (current && current->next) {
            if (current->relevance < current->next->relevance) {
                // Intercambia datos (no punteros por simplicidad)
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

                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);

    return list;
}

// Imprime estadísticas del grafo
void documentGraphPrint(DocumentGraph* graph) {
    if (!graph) return;

    printf("Estadísticas del Grafo de Documentos:\n");
    printf("===================================\n");
    printf("Nodos: %d\n", graph->nodeCount);
    printf("Aristas: %d\n", graph->edgeCount);

    printf("\nTop 10 documentos por grado de entrada:\n");

    // Ordenación simple para visualización
    GraphNode* nodes[10] = {NULL};
    GraphNode* current = graph->nodes;

    while (current) {
        for (int i = 0; i < 10; i++) {
            if (nodes[i] == NULL || current->indegree > nodes[i]->indegree) {
                for (int j = 9; j > i; j--) {
                    nodes[j] = nodes[j-1];
                }
                nodes[i] = current;
                break;
            }
        }
        current = current->next;
    }

    for (int i = 0; i < 10 && nodes[i] != NULL; i++) {
        printf("  %d. Documento %d: grado=%d, PageRank=%.4f\n",
               i+1, nodes[i]->documentId, nodes[i]->indegree, nodes[i]->pageRank);
    }
    printf("\n");
}

// Serializa las puntuaciones de relevancia a un archivo
int documentGraphSerializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename) {
    if (!graph || !documents || !filename) return 0;

    FILE* file = fopen(filename, "w");
    if (!file) return 0;

    Document* current = documents->head;
    while (current) {
        GraphNode* node = documentGraphFindNode(graph, current->id);
        if (node) {
            fprintf(file, "%d %.6f %.6f\n", node->documentId, (float)node->indegree, node->pageRank);
        }
        current = current->next;
    }

    fclose(file);
    return 1;
}

// Deserializa las puntuaciones de relevancia desde un archivo
int documentGraphDeserializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename) {
    if (!graph || !documents || !filename) return 0;

    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    int documentId;
    float indegree, pageRank;

    while (fscanf(file, "%d %f %f", &documentId, &indegree, &pageRank) == 3) {
        GraphNode* node = documentGraphFindNode(graph, documentId);
        if (node) {
            node->indegree = (int)indegree;
            node->pageRank = pageRank;
        }
    }

    fclose(file);
    return 1;
}