#define LINKEDLIST_IMPLEMENTATION
#include "./linkedlist.h"

char* print_val(void* value) {
    char* out = malloc(2 * sizeof(char));
    out[0] = ((char)(*(int*)value)) + '0';
    out[1] = '\0';
    return out;
}

int main(void) {
    int val = 0;
    Node* head = malloc(sizeof(Node));
    head->value = (void*)&val;
    head->next = NULL;
    SLList* list = create_list(head);
    int val2[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (size_t i = 0; i < 9; i++) {
        Node* in = malloc(sizeof(Node));
        in->value = (void*)&val2[i];
        in->next = NULL;
        append_node(list, in);
    }
    Node* in = malloc(sizeof(Node));
    in->value = (void*)&val2[0];
    in->next = NULL;
    insert_node(list, in, 3);
    print_list(list, &print_val);
    printf("\n");
    delete_node(list, 4);
    for (size_t i = 0; i < 15; i++) {
        deappend_node(list);
    }
    print_list(list, &print_val);
    return 0;
}