// query.h
#ifndef QUERY_H
#define QUERY_H

#include "document.h"

// Query item structure
typedef struct QueryItem {
    char* word;
    int isExclude;  // 0 for include, 1 for exclude
    struct QueryItem* next;
} QueryItem;

// Query structure
typedef struct Query {
    QueryItem* head;
    int itemCount;
} Query;

// Query history using a queue
typedef struct QueryHistoryNode {
    char* input;
    struct QueryHistoryNode* next;
} QueryHistoryNode;

typedef struct QueryHistory {
    QueryHistoryNode* front;
    QueryHistoryNode* rear;
    int count;
} QueryHistory;

// Function declarations
Query* queryInit(char* queryString);
QueryItem* queryItemCreate(char* word, int isExclude);
void queryAppendItem(Query* query, QueryItem* item);
void queryFree(Query* query);
void queryPrint(Query* query);

QueryHistory* queryHistoryCreate();
void queryHistoryEnqueue(QueryHistory* history, char* queryString);
void queryHistoryPrint(QueryHistory* history);
void queryHistoryFree(QueryHistory* history);

DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query);
int documentContainsWord(Document* doc, char* word);
char* cleanWord(char* word);
DocumentsList* documentsListCopy(DocumentsList* source);


#endif //QUERY_H
