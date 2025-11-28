#ifndef AST_H
#define AST_H

typedef enum {
    IDENTIFIER,
    INTEGER,   
    
    // Keywords
    PROGRAM,
    FUNC,
    LET,
    IF,
    WHILE,
    RETURN,

    BINOP
} NodeType;

typedef struct Node {
    NodeType node;

    // Literals / Identifiers
    char* name;      // For IDENTIFIER
    int value;       // For INTEGER
    Node* condition; // For if/while

    // Node Bodies
    Node** body;
    int body_count;

    // Statements and Expressions
    struct Node** children;
    int children_count;

    char op;        // Operator Type

    // 
} Node;

// Forward Declarations
Node* new_node(NodeType type);
Node* new_int_node(int value);
Node* new_identifier_node(const char* name);
Node* new_binop_node(char op, Node* left, Node* right);
void add_child(Node* parent, Node* child);
void free_ast(Node* node);

#endif