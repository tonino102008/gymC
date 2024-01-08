#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

struct Node {
    void* value;
    struct Node* next;
};

typedef struct {
    size_t len;
    Node* head;
} SLList;

SLList* create_list(Node* head);
void insert_node(SLList* list, Node* in, size_t pos);
void append_node(SLList* list, Node* in);
void delete_node(SLList* list, size_t pos);
void deappend_node(SLList* list);
void print_list(SLList* list, char*(*print_value)(void* value));

#endif // LINKEDLIST_H_

#ifdef LINKEDLIST_IMPLEMENTATION

SLList* create_list(Node* head) {
    SLList* list = malloc(sizeof(SLList));
    list->head = head;
    list->len = 0;
    return list;
}

void insert_node(SLList* list, Node* in, size_t pos) {
    assert(pos <= (list->len + 1));
    assert(pos != 0);
    assert(!(in->next));
    Node* head = list->head;
    for (size_t i = 0; i < pos - 1; i++) {
        head = head->next;
    }
    Node* tmp = head->next;
    in->next = tmp;
    head->next = in;
    list->len += 1;
}

void append_node(SLList* list, Node* in) {
    insert_node(list, in, list->len + 1);
}

void delete_node(SLList* list, size_t pos) {
    Node* head = list->head;
    if (!head->next) return;
    for (size_t i = 0; i < pos - 1; i++) {
        head = head->next;
    }
    Node* tmp = head->next;
    head->next = head->next->next;
    list->len -= 1;
    free(tmp);
}

void deappend_node(SLList* list) {
    delete_node(list, list->len);
}

void print_list(SLList* list, char*(*print_value)(void* value)) {
    Node* head = list->head;
    while (head->next) {
        printf("Current Value: %s\n", print_value(head->value));
        head = head->next;
    }
    printf("Current Value: %s\n", print_value(head->value));
}

#endif // LINKEDLIST_IMPLEMENTATION