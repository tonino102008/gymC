#ifndef TREEMAP_H_
#define TREEMAP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct TreeNode TreeNode;
typedef struct TreeMap TreeMap;

struct TreeNode {
    void* key;
    void* value;
    struct TreeNode* left;
    struct TreeNode* right;
};

struct TreeMap {
    size_t n_elem;
    TreeNode* head;
};

TreeMap* createTreeMap(TreeNode*);
void insertTreeNode(TreeMap*, TreeNode*, int(*compare_fun)(void*, void*));
void deleteTreeNode(TreeMap*, TreeNode*, int(*compare_fun)(void*, void*));
void travelTreeMap(TreeNode*, void(*generic_fun)(void*, void*));
void deleteTreeMap(TreeMap*);
void deleteRoutine(TreeNode*, size_t*);

#endif // TREEMAP_H_

#ifdef TREEMAP_IMPLEMENTATION

TreeMap* createTreeMap(TreeNode* head) {
    TreeMap* tree = malloc(sizeof(TreeMap));
    assert(head->right == NULL);
    assert(head->left == NULL);
    tree->head = malloc(sizeof(TreeNode));
    tree->head = head;
    tree->n_elem = 0;
    return tree;
}

void insertTreeNode(TreeMap* tree, TreeNode* node, int(*compare_fun)(void* key1, void* key2)) {
    TreeNode* head = tree->head;
    assert(node->right == NULL);
    assert(node->left == NULL);
    while (head->right || head->left) {
        int comp = compare_fun(node->key, head->key);
        if (comp == 0) return;
        if (comp > 0 && !head->right) break;
        if (comp < 0 && !head->left) break;
        if (comp > 0) head = head->right;
        if (comp < 0) head = head->left;
    }
    if (compare_fun(node->key, head->key) == 0) return;
    if (compare_fun(node->key, head->key) > 0) head->right = node;
    if (compare_fun(node->key, head->key) < 0) head->left = node;
    tree->n_elem += 1;
}

void deleteTreeNode(TreeMap* tree, TreeNode* node, int(*compare_fun)(void* key1, void* key2)) {
    TreeNode* head = tree->head;
    TreeNode* tmp = head;
    int comp = compare_fun(node->key, head->key);
    while (comp != 0) {
        if (!head->right && !head->left) return;
        if (comp > 0 && !head->right) return;
        if (comp < 0 && !head->left) return;
        tmp = head;
        if (comp > 0 && head->right) head = head->right;
        if (comp < 0 && head->left) head = head->left;
        comp = compare_fun(node->key, head->key);
    }
    TreeNode* right = head->right;
    TreeNode* left = head->left;
    if (compare_fun(node->key, tmp->right->key) == 0) {
        tmp->right = right;
        while (tmp->left) {
            tmp = tmp->left;
        }
        tmp->left = left;
    } else {
        printf("OK\n");
        tmp->left = left;
        while (tmp->right) {
            tmp = tmp->right;
        }
        tmp->right = right;
    }
    free(head->key);
    free(head->value);
    free(head);
    tree->n_elem -= 1;
}

void travelTreeMap(TreeNode* head, void(*generic_fun)(void* key, void* value)) {

    generic_fun(head->key, head->value);

    if (head->left) travelTreeMap(head->left, generic_fun);
    if (head->right) travelTreeMap(head->right, generic_fun);
}

void deleteTreeMap(TreeMap* tree) {
    
    deleteRoutine(tree->head, &(tree->n_elem));

    free(tree);
    tree = NULL;
    
}

void deleteRoutine(TreeNode* head, size_t* counter) {

    if (head->left) deleteRoutine(head->left, counter);
    if (head->right) deleteRoutine(head->right, counter);
    
    free(head->key);
    free(head->value);
    free(head);

}

#endif // TREEMAP_IMPLEMENTATION