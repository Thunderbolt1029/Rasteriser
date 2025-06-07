#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct Element_type {
    void *item;
    struct Element_type *next;
} Element;

Element* CreateList(void);
void ListAdd(Element* list, void* item);
void ListRemove(Element* list, void* item);
void* ListPop(Element* list);
void* ListGet(Element* list, int i);
void ListDelete(Element* list);

#endif
