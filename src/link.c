#include "link.h"
#include <stdio.h>
#include <stdlib.h>

// Crea un nuevo enlace
Link* linkCreate(int id) {
    Link* link = malloc(sizeof(Link));
    if (!link) return NULL;
    link->id = id;
    link->next = NULL;
    return link;
}

// Añade un enlace a la lista enlazada
void linkAppend(Link** head, int id) {
    Link* newLink = linkCreate(id);
    if (!newLink) return;

    if (*head == NULL) {
        *head = newLink;
        return;
    }

    // Verifica si el enlace ya existe
    Link* current = *head;
    while (current) {
        if (current->id == id) {
            free(newLink);
            return;
        }
        if (current->next == NULL) break;
        current = current->next;
    }

    current->next = newLink;
}

// Libera la memoria de la lista de enlaces
void linkFree(Link* head) {
    while (head) {
        Link* temp = head;
        head = head->next;
        free(temp);
    }
}
