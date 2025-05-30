# Makefile (Windows-compatible)
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
SOURCES=main.c document.c query.c hashmap.c graph.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=search_engine

# Windows executable extension
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del /Q
    MKDIR = mkdir
else
    RM = rm -f
    MKDIR = mkdir -p
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	$(RM) *.o $(TARGET) test_runner.exe *.cache 2>nul || echo.
else
	$(RM) $(OBJECTS) $(TARGET) test_runner *.cache
endif

datasets:
	$(MKDIR) datasets

# Note: clang-format might not be available on Windows
f:
	@echo "Formatting code..."
	@clang-format -i *.c *.h 2>nul || echo "clang-format not available"

# Note: valgrind is not available on Windows
v: $(TARGET)
	@echo "Memory check..."
ifeq ($(OS),Windows_NT)
	@echo "Valgrind not available on Windows. Run the program manually to check for issues."
	./$(TARGET)
else
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)
endif

test: test.c document.c query.c hashmap.c graph.c
	$(CC) $(CFLAGS) test.c document.c query.c hashmap.c graph.c -o test_runner -lm
	./test_runner

.PHONY: all clean f v test datasets