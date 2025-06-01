#ifndef QUERY_H
#define QUERY_H

//Estructura que representa un elemento consulta
typedef struct QueryItem {
    char* word;  //Palabra
    int isExclude;  //0 incluir 1 = excluir
    struct QueryItem* next; //Puntero siguiente elemento en la lista
} QueryItem;

//Estructura consulta, que almacena lista de elementos y num palabras
typedef struct Query {
    QueryItem* items;  //Primer elemento de la consulta
    int itemCount;  //Num total de elementos en la consulta (palabras)
} Query;

Query* queryInit(char* queryString); //Función inicializa consulta a partir de una cadena de texto
QueryItem* queryItemCreate(char* word, int isExclude); //Función crea nuevo elemento de consulta con una palabra y su indicador de inclusión/exclusión
void queryAppendItem(Query* query, QueryItem* item); //Función añade elemento a la consulta, ampliando la lista
void queryFree(Query* query); //Libera memoria utilizada por la consulta
void queryPrint(Query* query); //Imprime la consulta en pantalla, elementos y estado
char* cleanWord(char*word); //Limpia una palabra eliminando caracteres no deseados o espacios adicionales

#endif