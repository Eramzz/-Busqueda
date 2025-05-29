#ifndef DOCUMENT_H
#define DOCUMENT_H

typedef struct Link {
    int id; //Destinació
    struct Link* next;
} Link;

typedef struct Document {
    int id;
    char* title;
    char* body;
    Link* links;
    float relevancia;  //Per pagerank
    struct Document* next;
} Document;

typedef struct DocumentsList {
    Document* head;
    int size;
} DocumentsList;

//Funcions per Link
Link* linkCreate(int id); //Crea un nuevo enlace
void linkAppend(Link** head, int id); //Añade un enlace al final de la lista
void linkFree(Link* head); //Libera la memoria de la lista


//Funcions per Document
Document* documentDeserialize(char* path);
void documentPrint(Document* doc);
void documentFree(Document* doc);

//Funcions per DocumentsList
DocumentsList* documentsListCreate();
void documentsListAppend(DocumentsList* list, Document* doc);
void documentsListPrint(DocumentsList* list);
Document* documentsListGet(DocumentsList* list, int index);
void documentsListFree(DocumentsList* list);



#endif //DOCUMENT_H
