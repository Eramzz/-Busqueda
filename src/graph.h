// graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include "document.h"

// Graph node representing a document
typedef struct GraphNode {
    int documentId;
    int indegree;
    float pageRank;
    struct GraphEdge* edges;  // Outgoing edges
    struct GraphNode* next;   // For linked list of nodes
} GraphNode;

// Graph edge representing a link between documents
typedef struct GraphEdge {
    int destinationId;
    struct GraphEdge* next;
} GraphEdge;

// Document graph structure
typedef struct DocumentGraph {
    GraphNode* nodes;
    int nodeCount;
    int edgeCount;
} DocumentGraph;

// Function declarations
DocumentGraph* documentGraphCreate();
GraphNode* documentGraphFindNode(DocumentGraph* graph, int documentId);
GraphNode* documentGraphAddNode(DocumentGraph* graph, int documentId);
void documentGraphAddEdge(DocumentGraph* graph, int sourceId, int destinationId);
void documentGraphBuildFromDocuments(DocumentGraph* graph, DocumentsList* documents);
float documentGraphGetIndegree(DocumentGraph* graph, int documentId);
void documentGraphCalculatePageRank(DocumentGraph* graph, float dampingFactor, int maxIterations, float tolerance);
void documentGraphPrint(DocumentGraph* graph);
void documentGraphFree(DocumentGraph* graph);

// Enhanced document functions with relevance
void documentCalculateRelevance(Document* doc, DocumentGraph* graph);
DocumentsList* documentsListSortedDescending(DocumentsList* list);

// Cache management
int documentGraphSerializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename);
int documentGraphDeserializeRelevance(DocumentGraph* graph, DocumentsList* documents, char* filename);

#endif