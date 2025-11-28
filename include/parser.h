#ifndef PARSER_H
#define PARSER_H

#include <lexer.h>
#include <ast.h>

typedef struct {
    Lexer* lexer;
    Token current;
    Token previous;
} Parser;

// Forward Declarations
void parser_init(Parser* parser, Lexer* lexer);
Node** parse_program(Parser* parser);
Node** parse_statement(Parser* parser);
Node** parse_expression(Parser* parser);

#endif