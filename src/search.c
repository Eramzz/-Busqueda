#include "search.h"
#include "query.h"

//Función para revisar si un documento contiene una palabra específica
int documentContainsWord(Document* doc, char* word) { //busca tanto en el título como en el cuerpo del documento
    if (!doc || !word){
        return 0; //Si doc o palabra son no son validos no se puede buscar, devuelve 0
    }

    //Limpia el título del documento
    char* cleanedTitle = cleanWord(doc->title);
    if (cleanedTitle && strstr(cleanedTitle, word)) {
        free(cleanedTitle); //liberamos memoria usada por la copia
        return 1; //devuelve 1, la palabra está en el título
    }
    free(cleanedTitle); //libera memoria

    //Lo mismo con el cuerpo del documento
    char* cleanedBody = cleanWord(doc->body);
    if (cleanedBody && strstr(cleanedBody, word)) {
        free(cleanedBody);
        return 1; ///devuelve 1, la palabra está en el cuerpo
    }
    free(cleanedBody); //libera memoria

    return 0; //si no se ha dado ninguno de los 2 casos, la palabra no está ni en el título ni en el cuerpo
}

//Función crea copia de una lista de documentos (apunta a los mismos documentos pero en nueva lista)
DocumentsList* documentsListCopy(DocumentsList* source) { //entra puntero a la lista original (source)
    if (!source){
        return NULL; //si la lista es NULL, no existe, sale de la función
    }

    DocumentsList* copy = documentsListCreate(); //crea nueva lista vacía con documentsListCreate()
    if (!copy){
        return NULL; //si al crearla da error por falta de memoria devuelve NULL
    }

    Document* current = source->head; //puntero current para recorrer la lista original de documentos comenzando por head

    //recorre todos los documentos de la lista original
    while (current) {
        //Copiamos la estructura del documento
        Document* docCopy = malloc(sizeof(Document)); //gurda memoria para nuevo doc
        *docCopy = *current; //copia los punteros ID, título, cuerpo del doc original (apuntan al mismo sitio)
        docCopy->next = NULL; //establece next del nuevo doc a NULL para que no este reacionado con el original

        documentsListAppend(copy, docCopy); //añade documento copiado a la nueva lista
        current = current->next; //pasa al siguiente doc de la lista original para poder copiarlo y añadirlo a la lista
    }

    return copy; //devuelve la copia de la lista de doecuementos
}

//Función busca documentos que coincidan con una consulta (query)
DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query) {
    if (!documents || !query){
        return NULL; //comprueba que los punteros que le entran no sean NULL, sino sale y se ha acabado la búsqueda
    }

    DocumentsList* results = documentsListCreate(); //crea lista donde se guardarán los resultados que coinciden con la query
    if (!results){
        return NULL; //si ha dado error al crearse la lista sale de la función y devuelve NULL
    }

    Document* current = documents->head; //puntero current para recorrer lista de documentos original desde head

    //recorre todos los documentos de la lista original
    while (current) {
        int matches = 1; //inicializamos variable suponiendo que el documento es válido (1) hasta que se sea lo contrario

        QueryItem* queryItem = query->items; //puntero para recorrer términos de la consulta

        while (queryItem && matches) { //mientras el doc sea válido (1) sigue al bucle
            //mira si documento contiene la palabra current
            int contains = documentContainsWord(current, queryItem->word);

            if (queryItem->isExclude) {
                //si la palabra hay que excluir y esta en el documento, se descarta
                if (contains) {
                    matches = 0; //Entonces establece match a 0 pq no cumple la condición
                }

            } else {
                //Si la palabra estáincluida pero no esta tmb se descarta
                if (!contains) {
                    matches = 0; //No cumple con la condición
                }
            }

            queryItem = queryItem->next; //Pasa a sig palabra
        }

        if (matches) { //Si el documento cumple con todas las condiciones

            //copia documento original
            Document* docCopy = malloc(sizeof(Document));
            *docCopy = *current;
            docCopy->next = NULL;

            documentsListAppend(results, docCopy); //Se añade a la lista de resultados
        }

        current = current->next; //pasa a sig documento
    }

    return results; //devuelve los lista docs que coinciden con la busqueda
}