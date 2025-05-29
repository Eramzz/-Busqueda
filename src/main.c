#include "document.h"
#include "query.h"
#include "hashmap.h"
#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

DocumentsList* loadDocumentsFromDataset(const char* datasetPath) {
    DocumentsList* list = documentsListCreate();
    if (!list) return NULL;

    DIR* dir = opendir(datasetPath);
    if (!dir) {
        printf("Error: Could not open dataset directory '%s'\n", datasetPath);
        documentsListFree(list);
        return NULL;
    }

    struct dirent* entry;
    char filepath[512];

    while ((entry = readdir(dir)) != NULL) {
        // Skip directories and hidden files
        if (entry->d_name[0] == '.' || entry->d_type == DT_DIR) {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s/%s", datasetPath, entry->d_name);

        Document* doc = documentDeserialize(filepath);
        if (doc) {
            documentsListAppend(list, doc);
            printf("Loaded document: %s\n", doc->title);
        } else {
            printf("Warning: Failed to load document from %s\n", filepath);
        }
    }

    closedir(dir);
    return list;
}

void printSearchResults(DocumentsList* results, DocumentGraph* graph) {
    if (!results || results->size == 0) {
        printf("No documents found matching your query.\n\n");
        return;
    }

    // Calculate relevance for all results
    Document* current = results->head;
    while (current) {
        documentCalculateRelevance(current, graph);
        current = current->next;
    }

    // Sort results by relevance
    documentsListSortedDescending(results);

    printf("Search Results (%d found, sorted by relevance):\n", results->size);
    printf("================================================\n");

    current = results->head;
    int index = 0;
    int displayed = 0;

    while (current && displayed < 5) { // Show only first 5 results
        printf("%d. %s (ID: %d, Relevance: %.2f)\n",
               index, current->title, current->id, current->relevancia);

        // Print first 150 characters of body
        if (current->body) {
            int bodyLen = strlen(current->body);
            if (bodyLen > 150) {
                printf("   %.150s...\n", current->body);
            } else {
                printf("   %s\n", current->body);
            }
        }
        printf("\n");

        current = current->next;
        index++;
        displayed++;
    }

    if (results->size > 5) {
        printf("... and %d more results\n\n", results->size - 5);
    }
}

int main() {
    printf("Search Engine - Lab 4 (Document Graph & Relevance)\n");
    printf("=================================================\n\n");

    // Load documents from dataset
    DocumentsList* documents = loadDocumentsFromDataset("datasets");
    if (!documents || documents->size == 0) {
        printf("No documents loaded. Please ensure dataset files exist in 'datasets' directory.\n");
        if (documents) documentsListFree(documents);
        return 1;
    }

    printf("Loaded %d documents successfully.\n\n", documents->size);

    // Build document graph
    DocumentGraph* graph = documentGraphCreate();
    if (!graph) {
        printf("Error: Could not create document graph.\n");
        documentsListFree(documents);
        return 1;
    }

    // Try to load cached relevance scores
    if (documentGraphDeserializeRelevance(graph, documents, "relevance.cache")) {
        printf("Loaded cached relevance scores.\n");

        // Still need to build graph structure for new searches
        documentGraphBuildFromDocuments(graph, documents);
    } else {
        printf("Building document graph and calculating relevance...\n");

        clock_t graphStart = clock();
        documentGraphBuildFromDocuments(graph, documents);

        // Calculate PageRank
        documentGraphCalculatePageRank(graph, 0.85, 100, 0.0001);

        clock_t graphEnd = clock();
        double graphTime = ((double)(graphEnd - graphStart)) / CLOCKS_PER_SEC;
        printf("Graph analysis completed in %.3f seconds.\n", graphTime);

        // Cache relevance scores
        if (documentGraphSerializeRelevance(graph, documents, "relevance.cache")) {
            printf("Relevance scores cached successfully.\n");
        }
    }

    // Print graph statistics
    documentGraphPrint(graph);

    // Try to load reverse index from cache file
    ReverseIndex* reverseIndex = reverseIndexDeserialize("reverse_index.cache");

    if (reverseIndex) {
        printf("Loaded reverse index from cache.\n\n");
    } else {
        printf("Building new reverse index...\n");

        clock_t start = clock();
        reverseIndex = reverseIndexCreate();
        if (!reverseIndex) {
            printf("Error: Could not create reverse index.\n");
            documentGraphFree(graph);
            documentsListFree(documents);
            return 1;
        }

        reverseIndexBuild(reverseIndex, documents);
        clock_t end = clock();

        double buildTime = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Reverse index built in %.3f seconds.\n", buildTime);

        // Save to cache
        if (reverseIndexSerialize(reverseIndex, "reverse_index.cache")) {
            printf("Reverse index saved to cache.\n");
        }
        printf("\n");
    }

    // Initialize query history
    QueryHistory* history = queryHistoryCreate();

    // Main search loop
    char queryBuffer[201]; // Max 200 chars + null terminator

    while (1) {
        // Show recent searches
        queryHistoryPrint(history);

        printf("Enter search query (or empty to exit): ");
        fflush(stdout);

        if (!fgets(queryBuffer, sizeof(queryBuffer), stdin)) {
            break;
        }

        // Remove newline
        size_t len = strlen(queryBuffer);
        if (len > 0 && queryBuffer[len - 1] == '\n') {
            queryBuffer[len - 1] = '\0';
            len--;
        }

        // Check for empty query (exit condition)
        if (len == 0) {
            break;
        }

        // Add to history
        queryHistoryEnqueue(history, queryBuffer);

        // Parse query
        Query* query = queryInit(queryBuffer);
        if (!query) {
            printf("Error parsing query.\n\n");
            continue;
        }

        printf("\nParsed ");
        queryPrint(query);

        // Search documents using reverse index
        clock_t searchStart = clock();
        DocumentsList* results = reverseIndexSearch(reverseIndex, query, documents);
        clock_t searchEnd = clock();

        double searchTime = ((double)(searchEnd - searchStart)) / CLOCKS_PER_SEC;
        printf("Search completed in %.6f seconds.\n", searchTime);

        // Print results with relevance ranking
        printSearchResults(results, graph);

        // Allow user to select a document to view
        if (results && results->size > 0) {
            int choice;
            int maxChoice = (results->size < 5) ? results->size - 1 : 4;

            printf("Enter document index to view (0-%d), or -1 to search again: ", maxChoice);

            if (scanf("%d", &choice) == 1) {
                while (getchar() != '\n'); // Clear input buffer

                if (choice >= 0 && choice <= maxChoice) {
                    Document* selectedDoc = documentsListGet(results, choice);
                    if (selectedDoc) {
                        // Find original document (not the copy) for full details
                        Document* originalDoc = documents->head;
                        while (originalDoc) {
                            if (originalDoc->id == selectedDoc->id) {
                                // Calculate and display relevance
                                documentCalculateRelevance(originalDoc, graph);
                                documentPrint(originalDoc);

                                // Show additional graph info
                                float indegree = documentGraphGetIndegree(graph, originalDoc->id);
                                printf("Graph statistics:\n");
                                printf("  Indegree (incoming links): %.0f\n", indegree);
                                printf("  PageRank score: %.6f\n",
                                       documentGraphFindNode(graph, originalDoc->id)->pageRank);
                                printf("\n");
                                break;
                            }
                            originalDoc = originalDoc->next;
                        }
                    }
                }
            } else {
                while (getchar() != '\n'); // Clear input buffer
            }
        }

        // Cleanup
        queryFree(query);
        if (results) {
            // Free the result copies (shallow copies)
            Document* current = results->head;
            while (current) {
                Document* temp = current;
                current = current->next;
                free(temp); // Only free the copy, not the original document data
            }
            free(results);
        }

        printf("\n");
    }

    printf("Goodbye!\n");

    // Cleanup
    queryHistoryFree(history);
    reverseIndexFree(reverseIndex);
    documentGraphFree(graph);
    documentsListFree(documents);

    return 0;
}