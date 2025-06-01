#ifndef SEARCH_H
#define SEARCH_H

int documentContainsWord(Document* doc, char* word); //Función mira si en el documento está la palabra que buscamos (1 esta 0 no está)
DocumentsList* documentsListCopy(DocumentsList* source); //Función crea copia de una lista de documentos
DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query); //Función busca documentos que coincidan con la query que heos hecho

#endif//SEARCH_H
