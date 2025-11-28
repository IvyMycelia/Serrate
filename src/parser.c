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


//Parse a single expression to check what kind of token it is
Node* parse_expression(Parser* parser) {
    // If current token is an integer
    if (parser->current.type == INTEGER) {
        int value = atoi(parser->current.start);    // Convert string to int
        advance(parser);
        return new_int_node(value);
    } 
    // If token is an identifier
    else if (parser->current.type == IDENTIFIER) {
        char* name = strndup(parser->current.start, parser->current.length);
        advance(parser);
        return new_identifier_node(name);
    }
    // Unknown Expression
    else {
        fprintf(stderr, "Unexpected Token '%.*s' at Line: %d, Column: %d\n", 
            parser->current.length, parser->current.start, 
            parser->current.line, parser->current.column);
        advance(parser);
        return NULL;
    }
}


// Parse a single statement (minimal: let, if, while, return, func)
Node* parse_statement(Parser* parser) {
    Node* node = NULL;

    switch (parser->current.type) {
        case LET: {
            advance(parser);
            if (parser->current.type != IDENTIFIER) {
                fprintf(stderr, "Expected identifier after: `let`\n");
                return NULL;
            }
            char* name = strndup(parser->current.start, parser->current.length);
            advance(parser);

            if (!match(parser, ASSIGN)) {
                fprintf(stderr, "Expected '=' after identifier\n");
                return NULL;
            }

            Node* expression = parse_expression(parser);

            node = new_node(LET);
            add_child(node, new_identifier_node(name));
            add_child(node, expression);
            break;
        }

        case FUNC: {
            advance(parser);

            if (parser->current.type != IDENTIFIER) {
                fprintf(stderr, "Expected function name after `func`\n");
                return NULL;
            }

            char* name = strndup(parser->current.start, parser->current.length);
            advance(parser);

            node = new_node(FUNC);
            add_child(node, new_identifier_node(name)); // store function name

            node->body = NULL;
            node->body_count = 0;

            while (parser->current.type != END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == END) advance(parser);
            break;
        }

        case IF: {
            advance(parser);
            Node* condition = parse_expression(parser);

            node = new_node(IF);
            node->condition = condition;

            // Parse body until `end`
            node->body = NULL;
            node->body_count = 0;
            while (parser->current.type != END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == END) advance(parser);
            break;
        }

        case WHILE: {
            advance(parser);
            Node* condition = parse_expression(parser);

            node = new_node(WHILE);
            node->condition = condition;

            // Parse body until `end`
            node->body = NULL;
            node->body_count = 0;
            while (parser->current.type != END && parser->current.type != TOKEN_EOF) {
                Node* statement = parse_statement(parser);
                if (statement) add_child(node, statement);
            }

            if (parser->current.type == END) advance(parser);
            break;
        }

        case RETURN: {
            advance(parser);
            Node* expression = parse_expression(parser);

            node = new_node(RETURN);
            add_child(node, expression);
            break;
        }

        default: node = parse_expression(parser); break;
    }

    return node;
}


// Parse entire program (loops until TOKEN_EOF, adding statements to root node)
Node** parse_program(Parser* parser) {
    Node** program_nodes = NULL;
    int count = 0;

    while (parser->current.type != TOKEN_EOF) {
        Node* statement = parse_statement(parser);
        if (statement) {
            program_nodes = realloc(program_nodes, sizeof(Node*) * (count + 1));
            program_nodes[count++] = statement;
        }
    }

    return program_nodes;
}