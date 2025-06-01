#include "query.h"
#include <ctype.h>
#include <string.h>

//Función limpia una palabra y dejar solo letras y números (normaliza)
char* cleanWord(char* word) {
    if (!word){
        return NULL; //Si la palabra es NULL, no hay nada que limpiar
    }

    int len = strlen(word); //obtiene la longitud de la palabra original
    char* cleaned = malloc(len + 1); //reserva espacio para la versión limpia (+1 '\0')
    int j = 0; //indice para llenar la nueva cadena

    //recorre cada letra de la palabra original
    for (int i = 0; i < len; i++) {
        if (isalnum(word[i])) { //Si el carácter es una letra o un número...
            cleaned[j++] = tolower(word[i]); //lo agrega a la nueva cadena pero lo pasa a minúsculas
        }
    }
    cleaned[j] = '\0'; //Añade final \0

    return cleaned; //Devuelve la nueva palabra sin caracteres especiales
}

//Función para crear un nuevo ítem de consulta
QueryItem* queryItemCreate(char* word, int isExclude) {
    QueryItem* item = malloc(sizeof(QueryItem)); //guarda memoria para un nuevo ítem
    if (!item){
        return NULL; //Si la memoria no se pudo asignar, devuelve NULL
    }

    item->word = malloc(strlen(word) + 1); //guarda memoria para la palabra y la copia
    strcpy(item->word, word); //copia la palabra al nuevo ítem
    item->isExclude = isExclude; //guarda si la palabra debe excluirse en la búsqueda
    item->next = NULL; //inicializa el siguiente ítem como NULL (porque aún no tiene otro ítem después)

    return item; //retorna el ítem creado
}

//Función añade un ítem a la consulta
void queryAppendItem(Query* query, QueryItem* item) {
    if (!query || !item){
       return; //si la consulta o el ítem son NULL, no hay nada que agregar
    }

    if (query->items == NULL) { //si la consulta está vacía, asigna el ítem el primero
        query->items = item;
    } else {  //si ya hay ítems en la consulta, buscamos el último y lo agregamos ahí
        QueryItem* current = query->items; //Inicia en el primer ítem
        while (current->next) { //recorre hasta encontrar el último
            current = current->next;
        }
        current->next = item; //añade el nuevo ítem al final de la lista
    }
    query->itemCount++; //aumenta contador de ítems en la consulta
}

//Función inicializar una consulta desde un texto
Query* queryInit(char* queryString) {
    if (!queryString){
        return NULL; //Si la consulta está vacía, no hay nada que procesar, devuelve NULL
    }

    Query* query = malloc(sizeof(Query)); //reserva memoria para la consulta
    if (!query){
        return NULL; //si la memoria no se pudo reservar, retorna NULL
       }

    query->items = NULL; //inicializa la lista de ítems como vacía
    query->itemCount = 0; //inicializa el contador de ítems en 0

    //hace copia cadena de consulta para poder modificarla sin afectar la original
    char* queryStr = malloc(strlen(queryString) + 1);
    strcpy(queryStr, queryString);

    //divide la cadena en palabras usando espacios y tabulaciones como separadores
    char* token = strtok(queryStr, " \t\n");
    while (token != NULL) { //repite el proceso para cada palabra
        int isExclude = 0; //si es 0 hay que excluir la palabra
        char* word = token; //la palabra sin modificar

        //mira si la palabra empieza con '-'
        if (token[0] == '-' && strlen(token) > 1) {
            isExclude = 1; //marca la palabra como excluida
            word = token + 1; //salta el primer carácter ('-') para guardar solo la palabra
        }

        //Limpia la palabra antes de ponerla a la consulta (normaliza)
        char* cleanedWord = cleanWord(word);
        if (cleanedWord && strlen(cleanedWord) > 0) {
            QueryItem* item = queryItemCreate(cleanedWord, isExclude); //crea un ítem con la palabra
            if (item) { //si el ítem se ha creado bien lo añade a la consulta
                queryAppendItem(query, item);
            }
        }
        free(cleanedWord); //libera la memoria de la palabra limpia

        token = strtok(NULL, " \t\n"); //para tener la siguiente palabra
    }

    free(queryStr); //libera la memoria de la cadena que habia copiado
    return query; //devuelve la consulta
}

//Función imprime la consulta
void queryPrint(Query* query) {
    if (!query){
        return; //si la consulta está vacía no imprime nada
    }

    printf("Consulta: ");
    QueryItem* current = query->items; //empieza en el primer item
    while (current) { //cada ítem de la consulta
        if (current->isExclude) { //si el ítem es de exclusión, imprime con un '-' delante
            printf("-%s ", current->word);
        } else { //sino lo imprime sin cambios
            printf("%s ", current->word);
        }
        current = current->next; //pasa al siguiente ítem
    }
    printf("(%d items totales en tu consulta)\n", query->itemCount); //imprime el número total de items que ha hecho en la consulta
}

//Función liberar la memoria de la consulta
void queryFree(Query* query) {
    if (!query){
        return; //si la consulta está vacia no libera nada
    }

    QueryItem* current = query->items; //empieza primer ítem
    while (current) { //recorre todos los ítems de la consulta
        QueryItem* temp = current; //guarda la referencia del ítem actual
        current = current->next; //pasa al siguiente ítem antes de liberar el actual
        free(temp->word); //libera la memoria de la palabra
        free(temp); //Libera la memoria
    }
    free(query); //Libera memoria consulta
}
