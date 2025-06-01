#ifndef LINK_H
#define LINK_H

//Estructura que representa un link en una lista enlazada
typedef struct Link {
    int id;  //Identificador del link
    struct Link* next;  //Puntero al siguiente link de la lista
} Link;


Link* linkCreate(int id); //Función crea un nuevo enlace con ID que entra
void linkAppend(Link** head, int id); //Añade un nuevo link al final de la lista enlazada y si hace falta cambia el head
void linkFree(Link* head); //Libera la memoria lista enlazada

#endif //LINK_H