#ifndef AST_H
#define AST_H

typedef enum {
    AST_IDENTIFIER,
    AST_INTEGER,   
    
    // Keywords
    AST_PROGRAM,
    AST_FUNC,
    AST_LET,
    AST_IF,
    AST_WHILE,
    AST_RETURN,

    AST_BINOP,
    AST_UNARY
} NodeType;

typedef struct Node {
    NodeType node;

    // Literals / Identifiers
    char* name;      // For IDENTIFIER
    int value;       // For INTEGER
    struct Node* condition; // For if/while

    // Statements and Expressions
    struct Node** children;
    int children_count;

    // Body
    // struct Node** body;
    // int body_count;

    char op;        // Operator Type
} Node;

// Forward Declarations
Node* new_node(NodeType type);
Node* new_int_node(int value);
Node* new_identifier_node(const char* name);
Node* new_binop_node(char op, Node* left, Node* right);
void add_child(Node* parent, Node* child);
void free_ast(Node* node);

#endif