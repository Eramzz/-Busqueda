#include "reverse_index.h"

//Función normaliza una palabra (minúsculas, sin puntuación)
char* normalizeWord(char* word) {
    if (!word){
        return NULL; //si la palabra no es válida retorna NULL
    }

    int len = strlen(word); //obtiene la longitud de la palabra
    char* normalized = malloc(len + 1);  //reserva memoria para la palabra normalizada
    int j = 0; //índice para la nueva cadena

    for (int i = 0; i < len; i++) {
        if (isalnum(word[i])) {  //si el carácter es alfanumérico lo convierte a min y lo añade
            normalized[j++] = tolower(word[i]);
        }
    }
    normalized[j] = '\0';

    if (j == 0) { //si la palabra vacía
        free(normalized); //libera la memoria
        return NULL; //retorna null
    }

    return normalized; //devuelve palabra normalizada
}

//Fucnión extrae palabras de un texto y las añade al reverse index
void extractWordsFromText(char* text, ReverseIndex* index, int documentId) {
    if (!text || !index){
        return; //si el texto o el índice son nulos, no hace nada
    }

    char* textCopy = malloc(strlen(text) + 1); //crea una copia del texto
    strcpy(textCopy, text); //copia el texto

    char* token = strtok(textCopy, " \t\n\r.,!?;:()[]{}\"'-"); //divide en tokens (palabras) usando separadores
    while (token) { //mientras haya palabras
        char* normalized = normalizeWord(token); //normaliza la palabra
        if (normalized && strlen(normalized) > 0) { //si la palabra es válida la añade al índice
            hashmapPut(index->wordToDocuments, normalized, documentId);
            free(normalized); //libera memoria palabra normalizada
        }
        token = strtok(NULL, " \t\n\r.,!?;:()[]{}\"'-"); //siguiente palabra
    }

    free(textCopy); //libera copia del texto
}

//Función crea un reverse index
ReverseIndex* reverseIndexCreate() {
    ReverseIndex* index = malloc(sizeof(ReverseIndex)); //guarda memoria para el índice
    if (!index){
        return NULL; //si falla la asignación de memoria retorna NULL
    }
    index->wordToDocuments = hashmapCreate(1000); //crea hashmap con tamaño inicial 1000
    if (!index->wordToDocuments) { //si falla
        free(index); //libera el índice y devuelve NULL
        return NULL;
    }

    return index; //retorna el índice
}

// Construye el índice invertido a partir de documentos
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents) {
    if (!index || !documents){
        return; //si los parámetros son inválidos, termina
    }

    printf("Construyendo reverse index\n");

    Document* current = documents->head; //empieza con el primer documento
    int processed = 0; //contador para los documentos que procesa

    while (current) { //mientras haya documentos
        extractWordsFromText(current->title, index, current->id); //procesa el título
        extractWordsFromText(current->body, index, current->id); //procesa el cuerpo

        processed++; //incrementa contador
        if (processed % 10 == 0) { //cada 10 documentos muestra progreso
            printf("De momento %d documentos se han procesado y añadido\n", processed);
        }
        current = current->next; //Pasa al siguiente documento
    }
    printf("Reverse index construido, con %d entradas \n", index->wordToDocuments->count);
}

//Función busca documentos usando el reverse index
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments) {
    if (!index || !query || !allDocuments){
        return NULL; //si algún parámetro que le ha entrado no es válido sale
    }

    DocumentsList* results = documentsListCreate(); //crea la lista de resultados
    if (!results){
        return NULL; //si falla, retorna null
    }

    if (query->itemCount == 0) { //si no hay términos en la consulta
        return results; //devuelve lista vacía
    }

    QueryItem* currentItem = query->items; //current es primer ítem de la consulta
    DocumentIdList* candidateDocuments = NULL; //inicializa lista de candidatos a null de momento

    while (currentItem && currentItem->isExclude) { //se pasa los términos de exclusión al inicio
        currentItem = currentItem->next; //si ay que exluirlo pasa al siguiente
    }

    if (!currentItem) {
        return results; //si no hay términos de que incluir, vuelve vacía
    }

    candidateDocuments = hashmapGet(index->wordToDocuments, currentItem->word); //obtiene documentos para el primer término
    if (!candidateDocuments) {
        return results; //si no hay documentos, termina
    }

    int* candidateIds = malloc(candidateDocuments->count * sizeof(int)); //inicializa array de IDs
    int candidateCount = 0; //inicializa contador cantidad de candidatos

    DocumentIdNode* docNode = candidateDocuments->head; //comienza desde el principio, head
    while (docNode) { //recorre todos los nodos de la lista de documentos candidatos
        candidateIds[candidateCount++] = docNode->documentId; //guarda el ID del documento en el array y avanza el contador
        docNode = docNode->next; //pasa al siguiente nodo en la lista
    }

    currentItem = query->items; //vuelve al primer término de la consulta
    while (currentItem) { //recorre todos los términos de la consulta
        DocumentIdList* termDocuments = hashmapGet(index->wordToDocuments, currentItem->word); //obtiene la lista de documentos que contienen la palabra


        if (currentItem->isExclude) { //si es un término de exclusión (-cat)
            for (int i = 0; i < candidateCount; i++) { //recorre todos los candidatos
                if (candidateIds[i] != -1 && termDocuments && documentIdListContains(termDocuments, candidateIds[i])) {
                    //si el ID no está descartado y hay docs para este término y además doc contiene la palabra que queremos excluir
                    candidateIds[i] = -1; //marca el ID como descartado
                    }
            }
        } else { //si es incluido (cat)
            for (int i = 0; i < candidateCount; i++) { // recorre los IDs candidatos
                if (candidateIds[i] != -1 && (!termDocuments || !documentIdListContains(termDocuments, candidateIds[i]))) {
                    //si no está descartado ya y no está en los documentos que contienen este término
                    candidateIds[i] = -1; //lo descarta
                    }
            }
        }
        currentItem = currentItem->next; //pasa al siguiente item
    }

    for (int i = 0; i < candidateCount; i++) { //recorre todos los IDs candidatos previamente filtrados
        if (candidateIds[i] != -1) { //si el id no ha sido descartado (-1)
            Document* doc = allDocuments->head; //recorre la lista de todos los documentos originales

            while (doc) { //recorre docs lista
                if (doc->id == candidateIds[i]) { //si encuentra un documento que su ID coincida con candidato
                    Document* docCopy = malloc(sizeof(Document)); //guarda memoria para copiar el documento
                    *docCopy = *doc; //copia el contenido del documento original al nuevo
                    docCopy->next = NULL;

                    documentsListAppend(results, docCopy); //añade la copia del documento a la lista de resultados
                    break; //termina la búsqueda en la lista porque ya ha encontrado el doc
                }
                doc = doc->next; //pasa al siguiente doc en la lista original
            }
        }
    }

    free(candidateIds); //libera la memoria usada por el array de IDs candidatos, ya no se necesita
    return results; //devuelve la lista de documentos que coinciden con la consulta

}

//Función que Serializa el reverse index a un archivo
int reverseIndexSerialize(ReverseIndex* index, char* filename) {
    if (!index || !filename){
        return 0; //mira si los parámetros que le han entrado son válidos
    }

    FILE* file = fopen(filename, "w"); //abre archivo en modo escritura
    if (!file){
        return 0; // si falla, retorna 0
    }

    HashMap* map = index->wordToDocuments; //accede al hashmap

    for (int i = 0; i < map->size; i++) { //recorre los buckets
        HashMapEntry* entry = map->buckets[i];
        while (entry) {
            fprintf(file, "%s", entry->key); //escribe la palabra clave

            DocumentIdNode* docNode = entry->documentIds->head;
            while (docNode) { //escribe todos los IDs asociados al nodo
                fprintf(file, " %d", docNode->documentId);
                docNode = docNode->next;
            }
            fprintf(file, "\n"); //nueva línea por cada entrada

            entry = entry->next;
        }
    }

    fclose(file); //cierra el archivo
    return 1; //devuelve 1, ha sido un éxito
}

//Función deserializa el index desde un archivo
ReverseIndex* reverseIndexDeserialize(char* filename) {
    FILE* file = fopen(filename, "r"); //abre el archivo modo lectura
    if (!file){
        return NULL; //si fallaal abrirlo retorna null
    }

    ReverseIndex* index = reverseIndexCreate(); //crea un reverse index nuevo
    if (!index) {
        fclose(file);
        return NULL;
    }

    char line[1024]; //buffer line para líneas del archivo
    while (fgets(line, sizeof(line), file)) { //lee línea por línea
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0'; //elimina el salto de línea
        }

        char* word = strtok(line, " "); //primera palabra es la clave
        if (!word) continue;

        char* docIdStr = strtok(NULL, " "); //lee IDs
        while (docIdStr) {
            int docId = atoi(docIdStr); //convierte a num entero
            if (docId > 0) {
                hashmapPut(index->wordToDocuments, word, docId); //añade al índice
            }
            docIdStr = strtok(NULL, " "); //pasa a siguiente ID
        }
    }

    fclose(file); //cierra archivo
    return index; //devuelve índice construido
}

//Función libera la memoria del índice invertido
void reverseIndexFree(ReverseIndex* index) {
    if (!index){
        return; //si es null, no hace nada
    }

    hashmapFree(index->wordToDocuments); //libera hashmap
    free(index); //libera estructura principal
}