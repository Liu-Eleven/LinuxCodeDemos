


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#if 0

typedef int KEY_VALUE;

struct bstree_node {
	KEY_VALUE data;
	struct bstree_node *left;
	struct bstree_node *right;
};

struct bstree {
	struct bstree_node *root;
};

struct bstree_node *bstree_create_node(KEY_VALUE key) {
	struct bstree_node *node = (struct bstree_node*)malloc(sizeof(struct bstree_node));
	if (node == NULL) {
		assert(0);
	}
	node->data = key;
	node->left = node->right = NULL;

	return node;
}

int bstree_insert(struct bstree *T, int key) {

	assert(T != NULL);

	if (T->root == NULL) {
		T->root = bstree_create_node(key);
		return 0;
	}

	struct bstree_node *node = T->root;
	struct bstree_node *tmp = T->root;

	while (node != NULL) {
		tmp = node;
		if (key < node->data) {
			node = node->left;
		} else {
			node = node->right;
		}
	}

	if (key < tmp->data) {
		tmp->left = bstree_create_node(key);
	} else {
		tmp->right = bstree_create_node(key);
	}
	
	return 0;
}

int bstree_traversal(struct bstree_node *node) {
	if (node == NULL) return 0;
	
	bstree_traversal(node->left);
	printf("%4d ", node->data);
	bstree_traversal(node->right);
}


#define ARRAY_LENGTH		20
int main() {

	int keyArray[ARRAY_LENGTH] = {24,25,13,35,23, 26,67,47,38,98, 20,13,17,49,12, 21,9,18,14,15};

	struct bstree T = {0};
	int i = 0;
	for (i = 0;i < ARRAY_LENGTH;i ++) {
		bstree_insert(&T, keyArray[i]);
	}

	bstree_traversal(T.root);

	printf("\n");
}

#else

typedef int KEY_VALUE;


#define BSTREE_ENTRY(name, type) 	\
	struct name {					\
		struct type *left;			\
		struct type *right;			\
	}

struct bstree_node {
	KEY_VALUE data;
	BSTREE_ENTRY(, bstree_node) bst;
};

struct bstree {
	struct bstree_node *root;
};

struct bstree_node *bstree_create_node(KEY_VALUE key) {
	struct bstree_node *node = (struct bstree_node*)malloc(sizeof(struct bstree_node));
	if (node == NULL) {
		assert(0);
	}
	node->data = key;
	node->bst.left = node->bst.right = NULL;

	return node;
}

int bstree_insert(struct bstree *T, int key) {

	assert(T != NULL);

	if (T->root == NULL) {
		T->root = bstree_create_node(key);
		return 0;
	}

	struct bstree_node *node = T->root;
	struct bstree_node *tmp = T->root;

	while (node != NULL) {
		tmp = node;
		if (key < node->data) {
			node = node->bst.left;
		} else {
			node = node->bst.right;
		}
	}

	if (key < tmp->data) {
		tmp->bst.left = bstree_create_node(key);
	} else {
		tmp->bst.right = bstree_create_node(key);
	}
	
	return 0;
}

int bstree_traversal(struct bstree_node *node) {
	if (node == NULL) return 0;
	
	bstree_traversal(node->bst.left);
	printf("%4d ", node->data);
	bstree_traversal(node->bst.right);
}

#define ARRAY_LENGTH		20
int main() {

	int keyArray[ARRAY_LENGTH] = {24,25,13,35,23, 26,67,47,38,98, 20,13,17,49,12, 21,9,18,14,15};

	struct bstree T = {0};
	int i = 0;
	for (i = 0;i < ARRAY_LENGTH;i ++) {
		bstree_insert(&T, keyArray[i]);
	}

	bstree_traversal(T.root);

	printf("\n");
}


#endif

