CC = gcc
CFLAGS = -Wall -Wextra -g

# Archivos fuente
SRCS = main.c document.c document_list.c query.c hashmap.c graph.c link.c search.c reverse_index.c historial3.c
OBJS = $(SRCS:.c=.o)
TARGET = main

# Reglas principales
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Regla para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Reglas adicionales útiles
clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Regla para depuración
debug: CFLAGS += -g
debug: clean $(TARGET)

.PHONY: all clean run debug
