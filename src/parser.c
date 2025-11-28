#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "ast.h"


// Forward Declarations
static void advance(Parser* parser);
static int match(Parser* parser, TokenType type);


/*
    Advance the parser to the next token.
    Updates previous <— current, current <— Lexer_next()
*/
static void advance(Parser* parser) {
    parser->previous = parser->current;
    parser->current = Lexer_next(parser->lexer);
}


/*
    Check if current token matches `type` and consume it if so.
    Returns 1 if matched, 0 if not.
*/
static int match(Parser* parser, TokenType type) {
    if (parser->current.type == type) {
        advance(parser);
        return 1;
    }
    return 0;
}


// Initializes parser with the lexer.
void parser_init(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    parser->current = Lexer_next(lexer);
    parser->previous = parser->current;
}


// 
Node* parse_factor(Parser* parser) {
    if (parser->current.type == TOKEN_INTEGER) {
        int value = atoi(parser->current.start);    // Convert string to int
        advance(parser);
        return new_int_node(value);
    } 
    // If token is an identifier
    else if (parser->current.type == TOKEN_IDENTIFIER) {
        char* name = strndup(parser->current.start, parser->current.length);
        advance(parser);
        return new_identifier_node(name);
    }
    // If token is a '('
    else if (parser->current.type == TOKEN_LEFT_PAREN) {
        advance(parser);
        Node* expression = parse_expression(parser);
        if (!match(parser, TOKEN_RIGHT_PAREN)) {
            fprintf(stderr, "Expected ')' after expression\n");
            return NULL;
        }
        return expression;
    }
    else if (parser->current.type == TOKEN_MINUS) {
        advance(parser);
        Node* child = parse_factor(parser);
        Node* unary_node = new_node(AST_UNARY);
        unary_node->op = '-';
        add_child(unary_node, child);
        return unary_node;
    } else {
        fprintf(stderr, "Expected token '%.*s'\n", parser->current.length, parser->current.start);
        advance(parser);
        return NULL;
    }
}


// Handles Operators
Node* parse_term(Parser* parser) {
    Node* left = parse_factor(parser);
    while (parser->current.type == TOKEN_ASTERISK || parser->current.type == TOKEN_DIVISION) {
        char op = parser->current.type == TOKEN_ASTERISK ? '*' : '/';
        advance(parser);
        Node* right = parse_factor(parser);
        Node* binop_node = new_binop_node(op, left, right);
        left = binop_node;
    }
    return left;
}


//Parse a single expression to check what kind of token it is
Node* parse_expression(Parser* parser) {
    Node* left = parse_term(parser);  // handle *, / first
    while (parser->current.type == TOKEN_PLUS || parser->current.type == TOKEN_MINUS) {
        char op = parser->current.type == TOKEN_PLUS ? '+' : '-';
        advance(parser);
        Node* right = parse_term(parser);  // right side of +/-
        Node* binop_node = new_binop_node(op, left, right);
        left = binop_node;
    }
    return left;
}


// Parse a single statement (minimal: let, if, while, return, func)
Node* parse_statement(Parser* parser) {
    Node* node = NULL;

    switch (parser->current.type) {
        case TOKEN_LET: {
            advance(parser);
            if (parser->current.type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Expected identifier after: `let`\n");
                return NULL;
            }
            char* name = strndup(parser->current.start, parser->current.length);
            advance(parser);

            if (!match(parser, TOKEN_ASSIGN)) {
                fprintf(stderr, "Expected '=' after identifier\n");
                return NULL;
            }

            Node* expression = parse_expression(parser);

            node = new_node(AST_LET);
            add_child(node, new_identifier_node(name));
            add_child(node, expression);
            break;
        }

        case TOKEN_FUNC: {
            advance(parser);

            if (parser->current.type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Expected function name after `func`\n");
                return NULL;
            }

            char* name = strndup(parser->current.start, parser->current.length);
            advance(parser);

            node = new_node(AST_FUNC);
            add_child(node, new_identifier_node(name)); // store function name

            while (parser->current.type != TOKEN_END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == TOKEN_END) advance(parser);
            break;
        }

        case TOKEN_IF: {
            advance(parser);
            Node* condition = parse_expression(parser);

            node = new_node(AST_IF);
            node->condition = condition;

            // Parse body until `end`
            while (parser->current.type != TOKEN_END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == TOKEN_END) advance(parser);
            break;
        }

        case TOKEN_WHILE: {
            advance(parser);
            Node* condition = parse_expression(parser);

            node = new_node(AST_WHILE);
            node->condition = condition;

            // Parse body until `end`
            while (parser->current.type != TOKEN_END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == TOKEN_END) advance(parser);
            break;
        }

        case TOKEN_RETURN: {
            advance(parser);
            Node* expression = parse_expression(parser);

            node = new_node(AST_RETURN);
            add_child(node, expression);
            break;
        }

        default: node = parse_expression(parser); break;
    }

    return node;
}


// Parse entire program (loops until TOKEN_EOF, adding statements to root node)
Node* parse_program(Parser* parser) {
    Node* root = new_node(AST_PROGRAM);

    while (parser->current.type != TOKEN_EOF) {
        Node* statement = parse_statement(parser);
        if (statement) add_child(root, statement);
    }

    return root;
}


// Print out AST information
void print_ast(Node* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) printf("  "); // nicer indentation

    switch (node->node) {
        case AST_PROGRAM:       printf("PROGRAM\n"); break;
        case AST_IDENTIFIER:    printf("IDENTIFIER %s\n", node->name); break;
        case AST_INTEGER:       printf("INTEGER %d\n", node->value); break;

        case AST_FUNC:          
            printf("FUNC "); 
            if (node->children_count > 0 && node->children[0])
                printf("name=%s", node->children[0]->name);
            printf("\n");
            break;
        case AST_LET:           
            printf("LET "); 
            if (node->children_count > 0 && node->children[0])
                printf("%s = ...\n", node->children[0]->name);
            break;
        case AST_IF:            printf("IF\n"); break;
        case AST_WHILE:         printf("WHILE\n"); break;
        case AST_RETURN:        printf("RETURN\n"); break;

        case AST_BINOP:         printf("BINOP %c\n", node->op); break;
        case AST_UNARY:         printf("UNARY %c\n", node->op); break;
        default:                printf("UNKNOWN NODE\n"); break;
    }

    // Print condition if present
    if (node->condition) {
        for (int i = 0; i < indent + 1; i++) printf("  ");
        printf("Condition:\n");
        print_ast(node->condition, indent + 2);
    }

    // Print children (expressions/statements)
    for (int i = 0; i < node->children_count; i++)
        print_ast(node->children[i], indent + 1);

    // Print body (for FUNC/IF/WHILE)
    // for (int i = 0; i < node->body_count; i++)
    //     print_ast(node->body[i], indent + 1);
}
