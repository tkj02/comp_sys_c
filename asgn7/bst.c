#include "node.h"
#include "bst.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *bst_create(void) {
    // Returns empty tree
    return NULL;
}

#if 0
Node *bst_create(void) {
    // Allocates memory for node
    Node *n = (Node *) malloc(sizeof(Node));

    // Returns NULL (starting )
    if (n == NULL) {
        return n;
    }
    n->oldspeak = NULL;
    n->newspeak = NULL;
    n->left = NULL;
    n->right = NULL;
    return n;
}
#endif

// Helper function for bst_height
uint32_t max(uint32_t a, uint32_t b) {
    // Returns greatest value between a and b
    if (a >= b) {
        return a;
    }
    return b;
}

uint32_t bst_height(Node *root) {
    // If root is not NULL, returns max height + root height
    if (root) {
        return 1 + max(bst_height(root->left), bst_height(root->right));
    }

    // Root is NULL and the height is zero
    return 0;
}

uint32_t bst_size(Node *root) {
    // Keeps track of size
    uint32_t counter = 0;

    // Increases count for every recursive call
    if (root) {
        counter += 1;
        counter += bst_size(root->left);
        counter += bst_size(root->right);
    }
    return counter;
}

Node *bst_find(Node *root, char *oldspeak) {
    if (root == NULL) {
        return root;
    }

    // Current node matches oldspeak
    if (strcmp(root->oldspeak, oldspeak) == 0) {
        return root;
    }

    // Current node is greater than oldspeak
    else if (strcmp(root->oldspeak, oldspeak) > 0) {
        // Traverses to left of tree as new root
        return bst_find(root->left, oldspeak);
    }

    // Current node is less than oldspeak
    else if (strcmp(root->oldspeak, oldspeak) < 0) {
        // Traverses to right of tree as new root
        return bst_find(root->right, oldspeak);
    }

    // Oldspeak is not in tree
    else {
        return NULL;
    }
}

Node *bst_insert(Node *root, char *oldspeak, char *newspeak) {
    // Creates root of new bst or
    // creates new node in existing bst
    if (root == NULL) {
        return node_create(oldspeak, newspeak);
    }

    // If bst already exists:
#if 0
    // Checks if oldspeak and newspeak are in bst
    // If so, does not insert and returns NULL
    if (strcmp(root->oldspeak, oldspeak) == 0) {
        return root;
    }
#endif
    // Current node is greater than oldspeak
    if (strcmp(root->oldspeak, oldspeak) > 0) {
        // Traverses to left of tree as new root
        root->left = bst_insert(root->left, oldspeak, newspeak);
    }

    // Current node is lss than oldspeak
    else if (strcmp(root->oldspeak, oldspeak) < 0) {
        // Traverses to right of tree as new root
        root->right = bst_insert(root->right, oldspeak, newspeak);
    }

    else if (strcmp(root->oldspeak, oldspeak) == 0) {
        return root;
    }

    return root;
}

void bst_print(Node *root) {
    // If root is not NULL, prints nodes in inorder
    if (root) {
        bst_print(root->left);
        node_print(root);
        bst_print(root->right);
    }
}

void bst_delete(Node **root) {
    // Recursively frees node memory in postorder
    if (*root == NULL) {
        return;
    }
    bst_delete(&(*root)->left);
    bst_delete(&(*root)->right);
    node_delete(root);
    *root = NULL;
}
