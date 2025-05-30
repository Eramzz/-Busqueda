#ifndef SEARCH_H
#define SEARCH_H

#include "texto.h"

int documentContainsWord(Document* doc, char* word);
DocumentsList* documentsListCopy(DocumentsList* source);
DocumentsList* searchDocumentsWithQuery(DocumentsList* documents, Query* query);

#endif //SEARCH_H
