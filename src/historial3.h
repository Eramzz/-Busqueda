#ifndef HISTORIAL3_H
#define HISTORIAL3_H

typedef struct QueryHistoryNode {
    char* queryString; //input
    struct QueryHistoryNode* next;
} QueryHistoryNode;

typedef struct QueryHistory {
    QueryHistoryNode* front;
    QueryHistoryNode* rear;
    int count;
} QueryHistory;

QueryHistory* queryHistoryCreate();
void queryHistoryEnqueue(QueryHistory* history, char* queryString);
void queryHistoryPrint(QueryHistory* history);
void queryHistoryFree(QueryHistory* history);

#endif //HISTORIAL3_H
