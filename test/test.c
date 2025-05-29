// test.c (updated with hashmap tests)
#include "document.h"
#include "query.h"
#include "hashmap.h"
#include <assert.h>
#include <string.h>

// Simple test framework
int tests_run = 0;
int tests_passed = 0;

#define TEST(name) \
    printf("Running test: %s\n", name); \
    tests_run++;

#define ASSERT(condition) \
    if (condition) { \
        tests_passed++; \
        printf("  ✓ PASS\n"); \
    } else { \
        printf("  ✗ FAIL: %s\n", #condition); \
    }

// Test hashmap functionality
void test_hashmap_creation() {
    TEST("HashMap creation");

    HashMap* map = hashmapCreate(10);

    ASSERT(map != NULL);
    ASSERT(map->size == 10);
    ASSERT(map->count == 0);

    hashmapFree(map);
}

void test_hashmap_put_get() {
    TEST("HashMap put and get");

    HashMap* map = hashmapCreate(10);

    hashmapPut(map, "hello", 123);
    hashmapPut(map, "world", 456);
    hashmapPut(map, "hello", 789); // Same key, different document

    DocumentIdList* helloList = hashmapGet(map, "hello");
    DocumentIdList* worldList = hashmapGet(map, "world");
    DocumentIdList* nonExistent = hashmapGet(map, "nonexistent");

    ASSERT(helloList != NULL);
    ASSERT(helloList->count == 2);
    ASSERT(worldList != NULL);
    ASSERT(worldList->count == 1);
    ASSERT(nonExistent == NULL);

    hashmapFree(map);
}

void test_hashmap_duplicates() {
    TEST("HashMap duplicate document IDs");

    HashMap* map = hashmapCreate(10);

    hashmapPut(map, "test", 123);
    hashmapPut(map, "test", 123); // Duplicate should be ignored

    DocumentIdList* list = hashmapGet(map, "test");

    ASSERT(list != NULL);
    ASSERT(list->count == 1);
    ASSERT(list->head->documentId == 123);

    hashmapFree(map);
}

void test_reverse_index_search() {
    TEST("Reverse index search functionality");

    // Create test documents
    DocumentsList* docs = documentsListCreate();

    Document* doc1 = malloc(sizeof(Document));
    doc1->id = 1;
    doc1->title = malloc(20);
    strcpy(doc1->title, "Cat care guide");
    doc1->body = malloc(50);
    strcpy(doc1->body, "Cats need proper nutrition and love");
    doc1->links = NULL;
    doc1->next = NULL;
    doc1->relevance = 0.0;

    Document* doc2 = malloc(sizeof(Document));
    doc2->id = 2;
    doc2->title = malloc(20);
    strcpy(doc2->title, "Dog training");
    doc2->body = malloc(50);
    strcpy(doc2->body, "Dogs require training and exercise");
    doc2->links = NULL;
    doc2->next = NULL;
    doc2->relevance = 0.0;

    documentsListAppend(docs, doc1);
    documentsListAppend(docs, doc2);

    // Build reverse index
    ReverseIndex* index = reverseIndexCreate();
    reverseIndexBuild(index, docs);

    // Test search
    Query* query = queryInit("cat");
    DocumentsList* results = reverseIndexSearch(index, query, docs);

    ASSERT(results != NULL);
    ASSERT(results->count == 1);
    ASSERT(results->head->id == 1);

    queryFree(query);

    // Free results (shallow copies)
    Document* current = results->head;
    while (current) {
        Document* temp = current;
        current = current->next;
        free(temp);
    }
    free(results);

    reverseIndexFree(index);
    documentsListFree(docs);
}

// Previous tests (keep all existing tests)
void test_query_init() {
    TEST("Query initialization");

    Query* query = queryInit("hello world");

    ASSERT(query != NULL);
    ASSERT(query->itemCount == 2);
    ASSERT(strcmp(query->items->word, "hello") == 0);
    ASSERT(query->items->isExclude == 0);
    ASSERT(strcmp(query->items->next->word, "world") == 0);
    ASSERT(query->items->next->isExclude == 0);

    queryFree(query);
}

void test_query_exclude() {
    TEST("Query with exclusion");

    Query* query = queryInit("cat -dog");

    ASSERT(query != NULL);
    ASSERT(query->itemCount == 2);
    ASSERT(strcmp(query->items->word, "cat") == 0);
    ASSERT(query->items->isExclude == 0);
    ASSERT(strcmp(query->items->next->word, "dog") == 0);
    ASSERT(query->items->next->isExclude == 1);

    queryFree(query);
}

void test_query_empty() {
    TEST("Empty query");

    Query* query = queryInit("");

    ASSERT(query != NULL);
    ASSERT(query->itemCount == 0);
    ASSERT(query->items == NULL);

    queryFree(query);
}

void test_clean_word() {
    TEST("Word cleaning");

    char* cleaned = normalizeWord("Hello,");
    ASSERT(strcmp(cleaned, "hello") == 0);
    free(cleaned);

    cleaned = normalizeWord("WORLD!");
    ASSERT(strcmp(cleaned, "world") == 0);
    free(cleaned);

    cleaned = normalizeWord("test123");
    ASSERT(strcmp(cleaned, "test123") == 0);
    free(cleaned);
}

void test_linear_search() {
    TEST("Linear search functionality");

    // Create test documents
    DocumentsList* docs = documentsListCreate();

    Document* doc1 = malloc(sizeof(Document));
    doc1->id = 1;
    doc1->title = malloc(20);
    strcpy(doc1->title, "Cat Document");
    doc1->body = malloc(30);
    strcpy(doc1->body, "This is about cats");
    doc1->links = NULL;
    doc1->next = NULL;
    doc1->relevance = 0.0;

    Document* doc2 = malloc(sizeof(Document));
    doc2->id = 2;
    doc2->title = malloc(20);
    strcpy(doc2->title, "Dog Document");
    doc2->body = malloc(30);
    strcpy(doc2->body, "This is about dogs");
    doc2->links = NULL;
    doc2->next = NULL;
    doc2->relevance = 0.0;

    documentsListAppend(docs, doc1);
    documentsListAppend(docs, doc2);

    // Test search
    Query* query = queryInit("cat");
    DocumentsList* results = searchDocumentsWithQuery(docs, query);

    ASSERT(results != NULL);
    ASSERT(results->count == 1);
    ASSERT(results->head->id == 1);

    queryFree(query);

    // Free results (shallow copies)
    Document* current = results->head;
    while (current) {
        Document* temp = current;
        current = current->next;
        free(temp);
    }
    free(results);

    documentsListFree(docs);
}

// Test link functionality (existing tests)
void test_links() {
    TEST("Link creation and append");

    Link* head = NULL;
    linkAppend(&head, 123);
    linkAppend(&head, 456);
    linkAppend(&head, 123); // Duplicate should be ignored

    ASSERT(head != NULL);
    ASSERT(head->id == 123);
    ASSERT(head->next != NULL);
    ASSERT(head->next->id == 456);
    ASSERT(head->next->next == NULL);

    linkFree(head);
}

void test_links_duplicate() {
    TEST("Link duplicate prevention");

    Link* head = NULL;
    linkAppend(&head, 100);
    linkAppend(&head, 100);

    ASSERT(head != NULL);
    ASSERT(head->next == NULL); // Should only have one link

    linkFree(head);
}

void test_links_empty() {
    TEST("Link empty list");

    Link* head = NULL;
    linkFree(head); // Should not crash

    ASSERT(1); // Test passes if we reach here
}

// Test documents list functionality (existing tests)
void test_documents_list_creation() {
    TEST("Documents list creation");

    DocumentsList* list = documentsListCreate();
    ASSERT(list != NULL);
    ASSERT(list->head == NULL);
    ASSERT(list->count == 0);

    documentsListFree(list);
}

void test_documents_list_append() {
    TEST("Documents list append");

    DocumentsList* list = documentsListCreate();

    // Create a simple document
    Document* doc = malloc(sizeof(Document));
    doc->id = 1;
    doc->title = malloc(10);
    strcpy(doc->title, "Test Doc");
    doc->body = malloc(10);
    strcpy(doc->body, "Test body");
    doc->links = NULL;
    doc->next = NULL;
    doc->relevance = 0.0;

    documentsListAppend(list, doc);

    ASSERT(list->count == 1);
    ASSERT(list->head == doc);

    documentsListFree(list);
}

void test_documents_list_get() {
    TEST("Documents list get by index");

    DocumentsList* list = documentsListCreate();

    // Create two documents
    Document* doc1 = malloc(sizeof(Document));
    doc1->id = 1;
    doc1->title = malloc(10);
    strcpy(doc1->title, "Doc 1");
    doc1->body = malloc(10);
    strcpy(doc1->body, "Body 1");
    doc1->links = NULL;
    doc1->next = NULL;
    doc1->relevance = 0.0;

    Document* doc2 = malloc(sizeof(Document));
    doc2->id = 2;
    doc2->title = malloc(10);
    strcpy(doc2->title, "Doc 2");
    doc2->body = malloc(10);
    strcpy(doc2->body, "Body 2");
    doc2->links = NULL;
    doc2->next = NULL;
    doc2->relevance = 0.0;

    documentsListAppend(list, doc1);
    documentsListAppend(list, doc2);

    Document* retrieved = documentsListGet(list, 0);
    ASSERT(retrieved == doc1);

    retrieved = documentsListGet(list, 1);
    ASSERT(retrieved == doc2);

    retrieved = documentsListGet(list, 2);
    ASSERT(retrieved == NULL);

    documentsListFree(list);
}

// Test document parsing (existing test)
void test_document_parsing() {
    TEST("Document parsing basic functionality");

    // Create a temporary test file
    FILE* testFile = fopen("test_doc.txt", "w");
    if (testFile) {
        fprintf(testFile, "123\n");
        fprintf(testFile, "Test Document Title\n");
        fprintf(testFile, "This is a test document with a [link](456) in it.\n");
        fclose(testFile);

        Document* doc = documentDeserialize("test_doc.txt");

        ASSERT(doc != NULL);
        ASSERT(doc->id == 123);
        ASSERT(strcmp(doc->title, "Test Document Title") == 0);
        ASSERT(strstr(doc->body, "test document") != NULL);
        ASSERT(doc->links != NULL);
        ASSERT(doc->links->id == 456);

        documentFree(doc);
        remove("test_doc.txt");
    } else {
        printf("  Could not create test file\n");
    }
}

int main() {
    printf("Running Search Engine Tests\n");
    printf("============================\n\n");

    // Run all tests
    test_links();
    test_links_duplicate();
    test_links_empty();
    test_documents_list_creation();
    test_documents_list_append();
    test_documents_list_get();
    test_document_parsing();

    // Query tests
    test_query_init();
    test_query_exclude();
    test_query_empty();
    test_clean_word();
    test_linear_search();

    // New hashmap tests
    test_hashmap_creation();
    test_hashmap_put_get();
    test_hashmap_duplicates();
    test_reverse_index_search();

    printf("\nTest Results:\n");
    printf("=============\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100);

    return (tests_passed == tests_run) ? 0 : 1;
}