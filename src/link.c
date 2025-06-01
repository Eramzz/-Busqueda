#include "link.h"
#include <stdio.h>
#include <stdlib.h>

//nuevo link con ID especificado, reserva memoria para la estructura y devuelve un puntero a ella
Link* linkCreate(int id) {
    Link* link = malloc(sizeof(Link));
    if (!link) return NULL; //si falla devuelve NULL
    link->id = id;
    link->next = NULL;
    return link;
}

//Función añade un nuevo link  a la lista enlazada
//Si la lista está vacía es el head de la cola y si ya está no se añade el link
void linkAppend(Link** head, int id) {
    Link* newLink = linkCreate(id);
    if (!newLink) return; //Si falla no hace nada

    if (*head == NULL) { //Si head es null (vacía) se añade y este se convierte en head
        *head = newLink;
        return;
    }

    //Recorre la lista para comprobar si el enlace ya existe y encontrar el último elemento
    Link* current = *head;
    while (current) {
        if (current->id == id) { //Si el ID ya existe, libera la memoria así no se duplica
            free(newLink);
            return;
        }
        if (current->next == NULL) break;
        current = current->next;
    }

    //Añade nuevo link al final de la lista
    current->next = newLink;
}

//Libera la memoria de todos los links en la lista enlazada
void linkFree(Link* head) {
    while (head) {
        Link* temp = head;
        head = head->next;
        free(temp);
    }
}