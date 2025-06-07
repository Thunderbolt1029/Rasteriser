#include "list.h"

Element* CreateList(void) {
    Element* e = malloc(sizeof(Element));
    e->item = NULL;
    e->next = NULL;
    return e;
}
void ListAdd(Element* list, void* item) {
    while (list->next != NULL)
        list = list->next;
    list->next = malloc(sizeof(Element));
    list = list->next;
    list->next = NULL;
    list->item = item;
}
void ListRemove(Element* list, void* item) {
    Element* prev = NULL;
    while (list->item != item && list->next != NULL) {
        prev = list;        
        list = list->next;
    }
    if (list->item == item) {
        prev->next = list->next;
        free(list->item);
        free(list);
    }
}
void* ListPop(Element* list) {
    if (list->next == NULL)
        return NULL;
    while (list->next != NULL)
        list = list->next;
    void* item = list->item;
    ListRemove(list, item);
    return item;
}
void* ListGet(Element* list, int i) {
    if (i == -1)
        return list->item;
    if (list->next == NULL)
        return NULL;
    return ListGet(list->next, i-1);
}
void ListDelete(Element* list) {
    if (list->next != NULL)
        ListDelete(list->next);
    free(list->item);
    free(list);
}
