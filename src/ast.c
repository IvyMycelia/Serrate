#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

// Create a new Node wih given type
Node* new_node(NodeType type) {
    Node* node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Out of memory allocating Node\n");
        exit(1);
    }

    node->node = type;
    node->name = NULL;
    node->value = 0;
    node->condition = NULL;
    node->children = NULL;
    node->children_count = 0;
    node->op = 0;

    return node;
}

// Create an integer literal Node
Node* new_int_node(int value) {
    Node* node = new_node(AST_INTEGER);
    node->value = value;
    return node;
}

// Create an identifier Node
Node* new_identifier_node(const char* name) {
    Node* node = new_node(AST_IDENTIFIER);
    node->name = strdup(name);
    return node;
}

// Create a binary operator Node
Node* new_binop_node(char op, Node* left, Node* right) {
    Node* node = new_node(AST_BINOP);
    node->op = op;
    add_child(node, left);
    add_child(node, right);
    return node;
}

// Add a child Node to a parent Node
void add_child(Node* parent, Node* child) {
    parent->children = realloc(parent->children, sizeof(Node*) * (parent->children_count + 1));
    parent->children[parent->children_count++] = child;
}

// Recursively free allocated memory from AST
void free_ast(Node* node) {
    if (!node) return;

    // Fre name if it exists
    if (node->name) free(node->name);

    // Free any children
    for (int i = 0; i < node->children_count; i++) free_ast(node->children[i]);
    free(node->children);

    // Free any bodies
    // for (int i = 0; i < node->body_count; i++) free_ast(node->body[i]);
    // free(node->body);

    free(node);
}