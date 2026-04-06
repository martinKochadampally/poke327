#include <cstdlib>
#include <cstring>
#include <vector>

#include "heap.h"

#define HEIGHT 21
#define WIDTH 80

heap_item *ref_table[HEIGHT][WIDTH];

void heap_init(heap *h){
    h->min = NULL;
    h->num_elements = 0;
    memset(ref_table, 0, sizeof(ref_table));
}

/*
This method lazily inserts items into the heap taking
O(1) time.
Returns NULL if malloc fails.
*/
heap_item* heap_insert(heap *h, void *t, int key){
    heap_item *tmp;

    // Checking if malloc returns null.
    if (!(tmp = (heap_item*) malloc(sizeof (*tmp)))) return NULL;


    tmp->t = t;
    tmp->key = key;
    tmp->degree = 0;
    tmp->parent = NULL;
    tmp->children = NULL;

    if (!h->min) {
        h->min = tmp;
        tmp->left = tmp;
        tmp->right = tmp;
    } else {
        // min<-tmp->min.right
        tmp->right = h->min->right;
        tmp->left = h->min;

        // min->tmp<-min.right
        h->min->right->left = tmp;
        h->min->right = tmp;

        if (tmp->key < h->min->key) {
            h->min = tmp;
        }
    }

    h->num_elements++;

    return tmp;
}

/*
Stores the smallest value in the heap in the second param
void t, without removing it.

Returns -1 if the given ptr to the heap is NULL.
Returns 0 for a sucess.
*/
int heap_min(heap *h, void **t){
    if (!h) return -1;
    *t = h->min->t;
    return 0;
}

/*
Makes y a child of x.
*/
void heap_merge(heap *h, heap_item *y, heap_item *x) {
    y->left->right = y->right;
    y->right->left = y->left;

    y->parent = x;
    if (x->children == NULL) {
        x->children = y;
        y->left = y;
        y->right = y;
    } else {
        y->left = x->children;
        y->right = x->children->right;
        x->children->right->left = y;
        x->children->right = y;
    }
    
    x->degree++;
    y->mark = 0;
}

/*
This code ensures there is one root node of each
degree.
*/
int refactor(heap *h) {
    heap_item *deg_list[32] = {NULL};
    heap_item *start, *node, *x, *y, *tmp;
    int d;

    if (!(start = h->min)) return -1;

    int num_roots = 0;

    std::vector<heap_item*> roots(h->num_elements);
    node = start;
    do {
        roots[num_roots++] = node;
        node = node->right;
    } while (node != start);

    for (int i = 0; i < num_roots; i++) {
        x = roots[i];
        d = x->degree;
        
        while (deg_list[d] != NULL) {
            y = deg_list[d];
            if (x->key > y->key) {
                tmp = x;
                x = y;
                y = tmp;
            }
            heap_merge(h, y, x);
            deg_list[d] = NULL;
            d++;
        }
        deg_list[d] = x;
    }

    // Rebuild the root list from the A array and find the new min
    h->min = NULL;
    for (int i = 0; i < 32; i++) {
        if (deg_list[i] != NULL) {
            if (h->min == NULL) {
                h->min = deg_list[i];
                deg_list[i]->left = deg_list[i];
                deg_list[i]->right = deg_list[i];
            } else {
                deg_list[i]->left = h->min;
                deg_list[i]->right = h->min->right;
                h->min->right->left = deg_list[i];
                h->min->right = deg_list[i];
                if (deg_list[i]->key < h->min->key) {
                    h->min = deg_list[i];
                }
            }
        }
    }

    return 0;
}

/*
Extracts the node with the minimum key, then finds the new 
min after reorganizing the heap so there is only one node in
the root list of a cetain degree.
*/
int heap_extract_min(heap *h, void **t, int *cost){
    heap_item *old_min, *curr, *first, *last;

    if (!h || h->num_elements == 0 || !(old_min = h->min)) return -1;

    *t = old_min->t;
    *cost = old_min->key;

    // Move children to root list
    if (old_min->children) {
        first = curr = old_min->children;
        
        // setting parents to NULL.
        do {
            curr->parent = NULL;
            curr = curr->right;
        } while (curr != first);

        last = first->left;

        old_min->left->right = first;
        first->left = old_min->left;

        last->right = old_min->right;
        old_min->right->left = last;
    } else {
        // If no children
        old_min->left->right = old_min->right;
        old_min->right->left = old_min->left;
    }

    if (old_min == old_min->right) {
        h->min = NULL;
        h->num_elements = 0;
    } else {
        h->min = old_min->right;
        h->num_elements--;
        refactor(h);
    }

    free (old_min);
    return 0;
}

void cut(heap *h, heap_item *node, heap_item *parent) {
    if (node->right == node) {
        parent->children = NULL;
    } else {
        node->left->right = node->right;
        node->right->left = node->left;

        if (parent->children == node) {
            parent->children = node->right;
        }
    }
    parent->degree--;

    node->left = h->min;
    node->right = h->min->right;
    h->min->right->left = node;
    h->min->right = node;

    node->parent = NULL;
    node->mark = 0;
}

void recursive_cut(heap *h, heap_item *parent) {
    heap_item *grandparent = parent->parent;
    if (grandparent != NULL) {
        if (parent->mark == 0) {
            parent->mark = 1;
        } else {
            cut(h, parent, grandparent);
            recursive_cut(h, grandparent);
        }
    }
}

int decrease_key(heap *h, heap_item *node, int new_key) {
    heap_item *parent;

    if (!h || !node || new_key > node->key) {
        return -1;
    }

    node->key = new_key;
    parent = node->parent;

    if (parent != NULL && node->key < parent->key) {
        cut(h, node, parent);
        recursive_cut(h, parent);
    }

    if (node->key < h->min->key) {
        h->min = node;
    }

    return 0;
}

/*
Stores the number of elements in the heap into size ptr.

Returns -1 if h is null.
*/
int heap_size(heap *h, int *size){
    if (!h) return -1;

    *size = h->num_elements;

    return 0;
}


void heap_item_destroy_recursive(heap_item *n) {
    heap_item *curr, *next;
    if (!n) return;

    n->left->right = NULL; 

    curr = n;
    next = NULL;

    while (curr) {
        next = curr->right;
        if (curr->children) {
            heap_item_destroy_recursive(curr->children);
        }
        free(curr);
        curr = next;
    }
}

void heap_destroy(heap *h) {
    if (!h || !h->min) return;

    heap_item_destroy_recursive(h->min);

    h->min = NULL;
    h->num_elements = 0;
}