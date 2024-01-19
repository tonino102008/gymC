#define TREEMAP_IMPLEMENTATION
#include "./treemap.h"

TreeNode* ctorTreeNode(void* key, void* value) {
    TreeNode* out = malloc(sizeof(TreeNode));
    out->key = key; // SHOULD CALL APPROPRIATE CTOR FOR VOID* TYPE ELEMENT
    out->value = value; // SHOULD CALL APPROPRIATE CTOR FOR VOID* TYPE ELEMENT
    out->left = NULL;
    out->right = NULL;
}

void dtorTreeNode(TreeNode* node) {
    free(node->key); // SHOULD CALL APPROPRIATE DTOR FOR VOID* TYPE ELEMENT
    free(node->value); // SHOULD CALL APPROPRIATE DTOR FOR VOID* TYPE ELEMENT
    free(node);
}

void printTreeNode(void* key, void* value) {
    assert(key);
    assert(value);
    printf("Key %s - Value: %d\n", (char*)key, *((int*)value));
}

int compareTreeNode(void* key1, void* key2) {
    assert(key1);
    assert(key2);
    return strcmp((char*)key1, (char*)key2);
}

int main(int argc, char** argv) {

    unsigned int seed;
    FILE* urandom = fopen("/dev/urandom", "r");
    fread(&seed, sizeof(int), 1, urandom);
    fclose(urandom);

    srand(seed);

    int start = 0;
    TreeNode* head = malloc(sizeof(TreeNode));
    head->key = malloc(2 * sizeof(char));
    *((char*)(head->key)) = '5';
    *((char*)(head->key) + 1) = '\0';
    head->value = malloc(sizeof(int));
    *((int*)head->value) = start;
    head->left = NULL;
    head->right = NULL;

    TreeMap* tree = createEmptyTreeMap();
    insertTreeNode(tree, head, &compareTreeNode);

    int i = 0;

    while (i < 20) {
        TreeNode* node = malloc(sizeof(TreeNode));
        node->key = malloc(2 * sizeof(char));
        *((char*)(node->key)) = (rand() % 10) + '0';
        *((char*)(node->key) + 1) = '\0';
        node->value = malloc(sizeof(int));
        *((int*)node->value) = i + 1;
        node->right = NULL;
        node->left = NULL;
        insertTreeNode(tree, node, &compareTreeNode);
        i++;
    }

    travelTreeMap(tree->head, &printTreeNode);

    printf("\nTree elements: %zu\n\n", tree->n_elem);

    assert(tree->head->left->right);
    deleteTreeNode(tree, tree->head->left->right, &compareTreeNode);

    char keyF[2] = "4\0";
    TreeNode* find = findTreeNode(tree->head, (void*)(&keyF), &compareTreeNode);
    assert(find);

    printTreeNode(find->key, find->value);
    printf("\n");

    travelTreeMap(tree->head, &printTreeNode);
    
    printf("\nTree elements: %zu\n\n", tree->n_elem);

    deleteTreeMap(tree);

    return 0;
}