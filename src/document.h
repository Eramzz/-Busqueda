#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "link.h"

typedef struct Document {
    int id;
    char* title;
    char* body;
    Link* links;
    float relevance;
    struct Document* next;
} Document;

Document* documentDeserialize(char* path);
void documentFree(Document* doc);
void documentPrint(Document* doc);

#endif
