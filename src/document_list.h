#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H

#include "document.h"

typedef struct DocumentsList {
    Document* head;
    int count;
} DocumentsList;

typedef struct DocumentIdNode {
    int documentId;
    struct DocumentIdNode* next;
} DocumentIdNode;

typedef struct DocumentIdList {
    DocumentIdNode* head;
    int count;
} DocumentIdList;


DocumentsList* documentsListCreate();
void documentsListAppend(DocumentsList* list, Document* doc);
void documentsListPrint(DocumentsList* list);
Document* documentsListGet(DocumentsList* list, int index);
void documentsListFree(DocumentsList* list);

DocumentIdList* documentIdListCreate();
void documentIdListAppend(DocumentIdList* list, int documentId);
int documentIdListContains(DocumentIdList* list, int documentId);
void documentIdListFree(DocumentIdList* list);




#endif //DOCUMENT_LIST_H
