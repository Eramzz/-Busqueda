#ifndef REVERSE_INDEX_H
#define REVERSE_INDEX_H

typedef struct ReverseIndex {
    HashMap* wordToDocuments;
} ReverseIndex;

char* normalizeWord(char* word);
void extractWordsFromText(char* text, ReverseIndex* index, int documentId);

ReverseIndex* reverseIndexCreate();
void reverseIndexBuild(ReverseIndex* index, DocumentsList* documents);
void reverseIndexFree(ReverseIndex* index);
DocumentsList* reverseIndexSearch(ReverseIndex* index, Query* query, DocumentsList* allDocuments);

// Serialization functions
int reverseIndexSerialize(ReverseIndex* index, char* filename);
ReverseIndex* reverseIndexDeserialize(char* filename);

#endif //REVERSE_INDEX_H
