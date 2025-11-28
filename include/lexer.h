#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,

    // Keywords
    TOKEN_FUNC,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_END,
    TOKEN_LET,

    // Delimiters
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_COLON,

    // Operators
    TOKEN_ASSIGN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_DIVISION,

    TOKEN_NEWLINE,

    TOKEN_UNKNOWN,

    TOKEN_EOF
} TokenType;

typedef struct Token {
    TokenType type;
    const char* start;
    int length;
    int line, column;
} Token;

typedef struct Lexer {
    const char* start;
    const char* current;
    int line, column;
} Lexer;

// Forward Declarations
void Lexer_init(Lexer* lexer, const char* source);
Token Lexer_next(Lexer* lexer);
const char* token_type_name(TokenType type);

#endif