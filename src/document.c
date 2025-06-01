#include "document.h"
#include <ctype.h>

//Función que lee un archivo de texto y hace un Document en memoria con su info
Document* documentDeserialize(char* path) {
    FILE* file = fopen(path, "r"); //abre archivo modo lectura
    if (!file){
       return NULL; //si falla devuelve NULL
    }

    Document* doc = malloc(sizeof(Document)); //reserva espacio memoria para un nuevo Document
    if (!doc) {
        fclose(file);
        return NULL; //si falla, cierra el archivo y devuelve NULL
    }

    //Inicializa los campos del documento
    doc->links = NULL; //lista links
    doc->next = NULL; //apunta al sig documento (linked list)
    doc->relevance = 0.0; //relevancia inicial =0

    //Lee el ID del documento (1r num del archivo)
    if (fscanf(file, "%d\n", &doc->id) != 1) {
        free(doc);
        fclose(file);
        return NULL; //Si no lo encuentra y no lo puede leer, libera la memoria y sale
    }

    //Lee el título y lo almacena en tittlebuffer
    char titleBuffer[1024];
    if (!fgets(titleBuffer, sizeof(titleBuffer), file)){
        free(doc);
        fclose(file);
        return NULL; //si falla la lectura, libera, cierrra el docum y duvuelve NULL
    }


    size_t titleLen = strlen(titleBuffer); //calcula long titulo
    if (titleLen > 0 && titleBuffer[titleLen - 1] == '\n') {
        titleBuffer[titleLen - 1] = '\0';
        titleLen--;
    }


    doc->title = malloc(titleLen + 1); //reserva memoria para el título
    strcpy(doc->title, titleBuffer); //copia el título

    //lee el cuerpo (resto del archivo), para saber cuanto ocupará
    fseek(file, 0, SEEK_END); //va al final del archivo
    long fileSize = ftell(file); //obtiene tamaño total
    fseek(file, 0, SEEK_SET); // vuelve al inicio

    // Omite el ID y el título
    char temp[1024];
    fgets(temp, sizeof(temp), file); //salta a la linea del ID
    fgets(temp, sizeof(temp), file); //salta a linea del titulo

    long bodyStart = ftell(file); //guarda dónde empieza cuerpo texto
    long bodySize = fileSize - bodyStart; //calcula tamaño total

    doc->body = malloc(bodySize + 1); //guarda memoria para el cuerpo (+1 '\0')
    if (!doc->body) {
        free(doc->title);
        free(doc);
        fclose(file);
        return NULL; //si falla libera memoria, cierra el archivo y devuelve NULL
    }

    size_t bytesRead = fread(doc->body, 1, bodySize, file); //lee cuerpo aarchivo
    doc->body[bytesRead] = '\0';


    char* bodyPtr = doc->body; //Inicializa un puntero que recorrerá el texto del cuerpo buscando links

    while ((bodyPtr = strchr(bodyPtr, '[')) != NULL) { //busca el ppio [, cuando ya no encuentre más sale del bucle
        char* linkEnd = strchr(bodyPtr, ']'); //busca otros paréntesis para saber dónde empieza y dónde cierra
        char* linkStart = strchr(bodyPtr, '(');
        char* linkClose = strchr(bodyPtr, ')');

        if (linkEnd && linkStart && linkClose && linkStart > linkEnd && linkClose > linkStart) { //si los paréntesis de antes son válidos
            char linkIdStr[32]; //inicializa cadena carácteres con espacio para 31 +'\0'
            int linkIdLen = linkClose - linkStart - 1; //calcula cuántos caracteres hay entre ( y ) extrae ID
            if (linkIdLen > 0 && linkIdLen < 32) { //si no esta vacío o es menor que 32 carácteres de largo
                strncpy(linkIdStr, linkStart + 1, linkIdLen); //copia los caracteres del ID al búfer linkIdStr y lo convierte en una cadena
                linkIdStr[linkIdLen] = '\0'; //fin string

                int linkId = atoi(linkIdStr); //pasa a a num entero
                if (linkId > 0) {
                    linkAppend(&doc->links, linkId); //añade el ID a lista de links
                }
            }
            bodyPtr = linkClose + 1; //sigue buscando después del cierre de paréntesis
        } else {
            bodyPtr++; //si no encuentra patrón avanza al siguiente carácter
        }
    }
    //Cierra el archivo y retorna puntero al documento cargado
    fclose(file);
    return doc;
}


//Función imprime los detalles de un documento
void documentPrint(Document* doc) {
    if (!doc){
        return; //si el documento es NULL, no hay nada, sale de la función
    }

    //Imprime título, relevancia y el cuerpo del documento
    printf("\n Documento %d :\n", doc->id);
    printf("Titulo: %s\n", doc->title);
    printf("Relevancia: %.2f\n", doc->relevance);
    printf("\n Cuerpo:\n%s\n", doc->body);

    //si hay links, los recorre y imprime sus ID correspondientes
    if (doc->links) {
        printf("\n Links documentos: ");
        Link* current = doc->links; //puntero current que primero apunta al primer enlace lista
        while (current) { //mientras que current no sea NULL
            printf("%d", current->id); //Imprime el ID del doc
            if (current->next){ //si el link al que apunta currento no es el últio pone una coma
                printf(", ");
            }
            current = current->next; //pasa al siguiente nodo de la lista, el puntero current apunta al siguiente
        }
        printf("\n");
    }
    printf("\n");
}

//Función libera memoria documento
void documentFree(Document* doc) {
    if (!doc){
        return; //si el puntero es NULL sale pq así no hace falta liberar
    }
    free(doc->title); //libera t´tulo, cuerpo, links y el documento
    free(doc->body);
    linkFree(doc->links);
    free(doc);
}