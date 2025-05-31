#ifndef QUERY_H
#define QUERY_H


typedef struct QueryItem {
    char* word;
    int isExclude;  // 0 per include, 1 per exclude
    struct QueryItem* next;
} QueryItem;

typedef struct Query {
    QueryItem* items;
    int itemCount;
} Query;

Query* queryInit(char* queryString);
QueryItem* queryItemCreate(char* word, int isExclude);
void queryAppendItem(Query* query, QueryItem* item);
void queryFree(Query* query);
void queryPrint(Query* query);
char* cleanWord(char* word);



#endif