#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define MAX_SIZE 2000000
#include <stdlib.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

typedef struct tree {
    int key;
    unsigned char height;
    struct tree *left;
    struct tree *right;
} AVL_tree;

AVL_tree nodes[MAX_SIZE];
int node_index = 0;

unsigned char fix_height(AVL_tree *t) {
    t->height = max(t->right == NULL ? 0: t->right->height, t->left == NULL ? 0: t->left->height) + 1;
    return t->height;
}

int height(AVL_tree* node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

AVL_tree* left_Rotate(AVL_tree *q) {
    AVL_tree *p = q->right;
    q->right = p->left;
    p->left = q;
    fix_height(q);
    fix_height(p);
    return p;
}

AVL_tree* right_Rotate(AVL_tree *p) {
    AVL_tree *q = p->left;
    p->left = q->right;
    q->right = p;
    fix_height(p);
    fix_height(q);
    return q;
}

int balance_factor(AVL_tree *t) {
    if (t == NULL) {
        return 0;
    }
    return height(t->left) - height(t->right);
}

AVL_tree *balance(AVL_tree *root) {
    fix_height(root);
    if (balance_factor(root) == -2) {
        if (balance_factor(root->right) > 0) {
            root->right = right_Rotate(root->right);
        }
        return left_Rotate(root);
    }
    if (balance_factor(root) == 2) {
        if (balance_factor(root->left) < 0) {
            root->left = left_Rotate(root->left);
        }
        return right_Rotate(root);
    }
    return root;
}

AVL_tree* newList(int data) {
    if (node_index < MAX_SIZE) {
        AVL_tree* new_node = &nodes[node_index++];
        new_node->key = data;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->height = 1;
        return new_node;
    } else {
        return NULL;
    }
}

AVL_tree* insert(AVL_tree *p, int k) {
    if (p == NULL) {
        return newList(k);
    }
    if (k < p->key) {
        p->left = insert(p->left, k);
    } else {
		p->right = insert(p->right, k);
    }
	return balance(p);
}

int main() {
    FILE *in = fopen("in.txt", "r");

    if (in == NULL) {
        return 0;
    }

    int n;
    if (fscanf(in, "%d", &n) != 1) {
        fclose(in);
        return 0;
    }

    if (n == 0) {
        printf("0");
        fclose(in);
        return 0;
    }

    AVL_tree *root = NULL;

    for (int i = 0; i < n; ++i) {
        int a;
        if (fscanf(in, "%d", &a) != 1) {
            fclose(in);
            return 0;
        }
        root = insert(root, a);
    }

    printf("%d", height(root));
        
    fclose(in);
    
    return 0;
}