#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Create a new document graph
DocumentGraph* documentGraphCreate() {
    DocumentGraph* graph = malloc(sizeof(DocumentGraph));
    if (!graph) return NULL;

    graph->nodes = NULL;
    graph->nodeCount = 0;
    graph->edgeCount = 0;

    return graph;
}

// Find a node in the graph by document ID
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

// Add a node to the graph
GraphNode* documentGraphAddNode(DocumentGraph* graph, int documentId) {
    if (!graph) return NULL;

    // Check if node already exists
    GraphNode* existing = documentGraphFindNode(graph, documentId);
    if (existing) return existing;

    // Create new node
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

// Add an edge to the graph
void documentGraphAddEdge(DocumentGraph* graph, int sourceId, int destinationId) {
    if (!graph) return;

    // Ensure both nodes exist
    GraphNode* sourceNode = documentGraphAddNode(graph, sourceId);
    GraphNode* destinationNode = documentGraphAddNode(graph, destinationId);

    if (!sourceNode || !destinationNode) return;

    // Check if edge already exists
    struct GraphEdge* currentEdge = sourceNode->edges;
    while (currentEdge) {
        if (currentEdge->destinationId == destinationId) {
            return; // Edge already exists
        }
        currentEdge = currentEdge->next;
    }

    // Create new edge
    struct GraphEdge* edge = malloc(sizeof(struct GraphEdge));
    if (!edge) return;

    edge->destinationId = destinationId;
    edge->next = sourceNode->edges;
    sourceNode->edges = edge;

    // Increment indegree of destination node
    destinationNode->indegree++;
    graph->edgeCount++;
}

// Build graph from documents
void documentGraphBuildFromDocuments(DocumentGraph* graph, DocumentsList* documents) {
    if (!graph || !documents) return;

    printf("Building document graph...\n");

    Document* current = documents->head;
    int processedNodes = 0;

    // First pass: create all nodes
    while (current) {
        documentGraphAddNode(graph, current->id);
        processedNodes++;
        current = current->next;
    }

    printf("Created %d nodes.\n", graph->nodeCount);

    // Second pass: create edges from links
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

    printf("Created %d edges.\n", graph->edgeCount);
    printf("Document graph built successfully.\n");
}

// Get indegree of a document (simple relevance score)
float documentGraphGetIndegree(DocumentGraph* graph, int documentId) {
    if (!graph) return 0.0;

    GraphNode* node = documentGraphFindNode(graph, documentId);
    if (!node) return 0.0;

    return (float)node->indegree;
}

// Calculate PageRank for all nodes
void documentGraphCalculatePageRank(DocumentGraph* graph, float dampingFactor, int maxIterations, float tolerance) {
    if (!graph || graph->nodeCount == 0) return;

    printf("Calculating PageRank (damping=%.2f, maxIter=%d, tol=%.6f)...\n",
           dampingFactor, maxIterations, tolerance);

    // Initialize PageRank values
    float initialRank = 1.0 / graph->nodeCount;
    GraphNode* current = graph->nodes;
    while (current) {
        current->pageRank = initialRank;
        current = current->next;
    }

    // Iterative calculation
    for (int iteration = 0; iteration < maxIterations; iteration++) {
        // Create array to store new PageRank values
        float* newRanks = calloc(graph->nodeCount, sizeof(float));
        GraphNode** nodeArray = malloc(graph->nodeCount * sizeof(GraphNode*));

        // Build node array for easier access
        current = graph->nodes;
        int index = 0;
        while (current) {
            nodeArray[index] = current;
            newRanks[index] = (1.0 - dampingFactor) / graph->nodeCount;
            index++;
            current = current->next;
        }

        // Calculate new PageRank values
        for (int i = 0; i < graph->nodeCount; i++) {
            GraphNode* node = nodeArray[i];
            struct GraphEdge* edge = node->edges;

            // Count outgoing edges
            int outDegree = 0;
            while (edge) {
                outDegree++;
                edge = edge->next;
            }

            if (outDegree > 0) {
                edge = node->edges;
                while (edge) {
                    // Find destination node index
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

        // Check for convergence
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
            printf("PageRank converged after %d iterations (diff=%.6f).\n", iteration + 1, maxDiff);
            return;
        }

        if ((iteration + 1) % 10 == 0) {
            printf("Iteration %d, max diff: %.6f\n", iteration + 1, maxDiff);
        }
    }

    printf("PageRank completed %d iterations (did not converge).\n", maxIterations);
}

// Print graph statistics
void documentGraphPrint(DocumentGraph* graph) {
    if (!graph) return;

    printf("Document Graph Statistics:\n");
    printf("=========================\n");
    printf("Nodes: %d\n", graph->nodeCount);
    printf("Edges: %d\n", graph->edgeCount);

    printf("\nTop 10 documents by indegree:\n");

    // Simple sorting for display (bubble sort for small datasets)
    GraphNode* nodes[10] = {NULL}; // Top 10
    GraphNode* current = graph->nodes;

    while (current) {
        // Insert into top 10 if applicable
        for (int i = 0; i < 10; i++) {
            if (nodes[i] == NULL || current->indegree > nodes[i]->indegree) {
                // Shift others down
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
        printf("  %d. Document %d: indegree=%d, pagerank=%.4f\n",
               i+1, nodes[i]->documentId, nodes[i]->indegree, nodes[i]->pageRank);
    }
    printf("\n");
}

// Calculate relevance for a document using graph
void documentCalculateRelevance(Document* doc, DocumentGraph* graph) {
    if (!doc || !graph) return;

    GraphNode* node = documentGraphFindNode(graph, doc->id);
    if (node) {
        // Use PageRank if available, otherwise use indegree
        if (node->pageRank > 0.0) {
            doc->relevancia = node->pageRank * 1000.0; // Scale for display
        } else {
            doc->relevancia = (float)node->indegree;
        }
    } else {
        doc->relevancia = 0.0;
    }
}

// Sort documents list by relevance (descending)
DocumentsList* documentsListSortedDescending(DocumentsList* list) {
    if (!list || list->size <= 1) return list;

    // Simple bubble sort (adequate for small result sets)
    int swapped;
    do {
        swapped = 0;
        Document* current = list->head;

        while (current && current->next) {
            if (current->relevancia < current->next->relevancia) {
                // Swap documents (swap data, not pointers for simplicity)
                int tempId = current->id;
                char* tempTitle = current->title;
                char* tempBody = current->body;
                Link* tempLinks = current->links;
                float tempRelevance = current->relevancia;

                current->id = current->next->id;
                current->title = current->next->title;
                current->body = current->next->body;
                current->links = current->next->links;
                current->relevancia = current->next->relevancia;

                current->next->id = tempId;
                current->next->title = tempTitle;
                current->next->body = tempBody;
                current->next->links = tempLinks;
                current->next->relevancia = tempRelevance;

                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);

    return list;
}

// Serialize relevance scores to file
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

// Deserialize relevance scores from file
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

// Free the document graph
void documentGraphFree(DocumentGraph* graph) {
    if (!graph) return;

    GraphNode* current = graph->nodes;
    while (current) {
        GraphNode* nodeToFree = current;
        current = current->next;

        // Free edges
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