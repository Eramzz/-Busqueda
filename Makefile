# Makefile (updated for Lab 4)
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g -lm
SOURCES=main.c document.c query.c hashmap.c graph.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=search_engine

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) test_runner *.cache

f:
	clang-format -i *.c *.h

v: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

test: test.c document.c query.c hashmap.c graph.c
	$(CC) $(CFLAGS) test.c document.c query.c hashmap.c graph.c -o test_runner -lm
	./test_runner

.PHONY: all clean f v test
