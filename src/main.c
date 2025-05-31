#include "document_list.c"
#include "query.c"
#include "document.c"
#include "historial3.c"
#include "reverse_index.c"
#include "hashmap.c"
#include "graph.c"
#include "link.c"
#include "search.c"

#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>

int isDirectory(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return 0;
    }
    return S_ISDIR(statbuf.st_mode);
}

// Carga documentos desde un directorio de dataset
DocumentsList* loadDocumentsFromDataset(const char* datasetPath) {
    DocumentsList* list = documentsListCreate();
    if (!list) return NULL;

    DIR* dir = opendir(datasetPath);
    if (!dir) {
        printf("Error: No se pudo abrir el directorio del dataset '%s'\n", datasetPath);
        documentsListFree(list);
        return NULL;
    }

    struct dirent* entry;
    char filepath[512];

    while ((entry = readdir(dir)) != NULL) {
        // Ignora archivos ocultos y entradas de directorio
        if (entry->d_name[0] == '.') {
            continue;
        }

        // Construye la ruta completa
        snprintf(filepath, sizeof(filepath), "%s/%s", datasetPath, entry->d_name);

        // Ignora subdirectorios
        if (isDirectory(filepath)) {
            continue;
        }

        Document* doc = documentDeserialize(filepath);
        if (doc) {
            documentsListAppend(list, doc);
            printf("Documento cargado: %s\n", doc->title);
        } else {
            printf("Advertencia: Error al cargar documento desde %s\n", filepath);
        }
    }

    closedir(dir);
    return list;
}

// Muestra los resultados de búsqueda
void printSearchResults(DocumentsList* results, DocumentGraph* graph) {
    if (!results || results->count == 0) {
        printf("No se encontraron documentos que coincidan con la búsqueda.\n\n");
        return;
    }

    // Calcula relevancia para todos los resultados
    Document* current = results->head;
    while (current) {
        documentCalculateRelevance(current, graph);
        current = current->next;
    }

    // Ordena por relevancia
    documentsListSortedDescending(results);

    printf("Resultados (%d encontrados, ordenados por relevancia):\n", results->count);
    printf("================================================\n");

    current = results->head;
    int index = 0;
    int displayed = 0;

    // Muestra solo los primeros 5 resultados
    while (current && displayed < 5) {
        printf("%d. %s (ID: %d, Relevancia: %.2f)\n",
               index, current->title, current->id, current->relevance);

        // Muestra los primeros 150 caracteres del cuerpo
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

    if (results->count > 5) {
        printf("... y %d resultados más\n\n", results->count - 5);
    }
}

int main() {
    printf("Motor de Búsqueda \n");
    printf("=================================================\n\n");

    // Carga documentos
DocumentsList* documents = loadDocumentsFromDataset("datasets/wikipedia12/");
    if (!documents || documents->count == 0) {
        printf("No se cargaron documentos. Verifica que existan archivos en el directorio 'datasets'.\n");
        if (documents) documentsListFree(documents);
        return 1;
    }

    printf("Se cargaron %d documentos correctamente.\n\n", documents->count);

    // Construye el grafo de documentos
    DocumentGraph* graph = documentGraphCreate();
    if (!graph) {
        printf("Error: No se pudo crear el grafo de documentos.\n");
        documentsListFree(documents);
        return 1;
    }

    // Intenta cargar puntuaciones de relevancia desde caché
    if (documentGraphDeserializeRelevance(graph, documents, "relevance.cache")) {
        printf("Se cargaron puntuaciones de relevancia desde caché.\n");

        // Aún necesita construir la estructura del grafo para nuevas búsquedas
        documentGraphBuildFromDocuments(graph, documents);
    } else {
        printf("Construyendo grafo de documentos y calculando relevancia...\n");

        clock_t graphStart = clock();
        documentGraphBuildFromDocuments(graph, documents);

        // Calcula PageRank
        documentGraphCalculatePageRank(graph, 0.85, 100, 0.0001);

        clock_t graphEnd = clock();
        double graphTime = ((double)(graphEnd - graphStart)) / CLOCKS_PER_SEC;
        printf("Análisis del grafo completado en %.3f segundos.\n", graphTime);

        // Guarda en caché
        if (documentGraphSerializeRelevance(graph, documents, "relevance.cache")) {
            printf("Puntuaciones de relevancia guardadas en caché.\n");
        }
    }

    // Muestra estadísticas del grafo
    documentGraphPrint(graph);

    // Intenta cargar el índice invertido desde caché
    ReverseIndex* reverseIndex = reverseIndexDeserialize("reverse_index.cache");

    if (reverseIndex) {
        printf("Índice invertido cargado desde caché.\n\n");
    } else {
        printf("Construyendo nuevo índice invertido...\n");

        clock_t start = clock();
        reverseIndex = reverseIndexCreate();
        if (!reverseIndex) {
            printf("Error: No se pudo crear el índice invertido.\n");
            documentGraphFree(graph);
            documentsListFree(documents);
            return 1;
        }

        reverseIndexBuild(reverseIndex, documents);
        clock_t end = clock();

        double buildTime = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Índice invertido construido en %.3f segundos.\n", buildTime);

        // Guarda en caché
        if (reverseIndexSerialize(reverseIndex, "reverse_index.cache")) {
            printf("Índice invertido guardado en caché.\n");
        }
        printf("\n");
    }

    // Historial de búsquedas
    QueryHistory* history = queryHistoryCreate();

    // Bucle principal de búsqueda
    char queryBuffer[201]; // Máximo 200 caracteres

    while (1) {
        // Muestra búsquedas recientes
        queryHistoryPrint(history);

        printf("Ingresa tu búsqueda (o vacío para salir): ");
        fflush(stdout);

        if (!fgets(queryBuffer, sizeof(queryBuffer), stdin)) {
            break;
        }

        // Elimina salto de línea
        size_t len = strlen(queryBuffer);
        if (len > 0 && queryBuffer[len - 1] == '\n') {
            queryBuffer[len - 1] = '\0';
            len--;
        }

        // Salir si la entrada está vacía
        if (len == 0) {
            break;
        }

        // Añade al historial
        queryHistoryEnqueue(history, queryBuffer);

        // Procesa la consulta
        Query* query = queryInit(queryBuffer);
        if (!query) {
            printf("Error al procesar la consulta.\n\n");
            continue;
        }

        printf("\nConsulta procesada: ");
        queryPrint(query);

        // Busca documentos
        clock_t searchStart = clock();
        DocumentsList* results = reverseIndexSearch(reverseIndex, query, documents);
        clock_t searchEnd = clock();

        double searchTime = ((double)(searchEnd - searchStart)) / CLOCKS_PER_SEC;
        printf("Búsqueda completada en %.6f segundos.\n", searchTime);

        // Muestra resultados
        printSearchResults(results, graph);

        // Permite seleccionar un documento para ver detalles
        if (results && results->count > 0) {
            int choice;
            int maxChoice = (results->count < 5) ? results->count - 1 : 4;

            printf("Ingresa el índice del documento a ver (0-%d), o -1 para buscar de nuevo: ", maxChoice);

            if (scanf("%d", &choice) == 1) {
                while (getchar() != '\n'); // Limpia el buffer de entrada

                if (choice >= 0 && choice <= maxChoice) {
                    Document* selectedDoc = documentsListGet(results, choice);
                    if (selectedDoc) {
                        // Busca el documento original (no la copia)
                        Document* originalDoc = documents->head;
                        while (originalDoc) {
                            if (originalDoc->id == selectedDoc->id) {
                                // Muestra detalles completos
                                documentCalculateRelevance(originalDoc, graph);
                                documentPrint(originalDoc);

                                // Muestra información del grafo
                                float indegree = documentGraphGetIndegree(graph, originalDoc->id);
                                GraphNode* node = documentGraphFindNode(graph, originalDoc->id);
                                printf("Estadísticas del grafo:\n");
                                printf("  Grado de entrada (enlaces entrantes): %.0f\n", indegree);
                                if (node) {
                                    printf("  Puntuación PageRank: %.6f\n", node->pageRank);
                                }
                                printf("\n");
                                break;
                            }
                            originalDoc = originalDoc->next;
                        }
                    }
                }
            } else {
                while (getchar() != '\n'); // Limpia el buffer de entrada
            }
        }

        // Libera memoria
        queryFree(query);
        if (results) {
            Document* current = results->head;
            while (current) {
                Document* temp = current;
                current = current->next;
                free(temp); // Libera solo la copia
            }
            free(results);
        }

        printf("\n");
    }

    printf("¡Hasta luego!\n");

    // Libera recursos
    queryHistoryFree(history);
    reverseIndexFree(reverseIndex);
    documentGraphFree(graph);
    documentsListFree(documents);

    return 0;
}