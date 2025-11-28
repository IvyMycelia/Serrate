#ifndef LEXER_H
#define LEXER_H

typedef enum TokenType {
    IDENTIFIER,
    INTEGER,

    // Keywords
    FUNC,
    IF,
    WHILE,
    RETURN,
    END,
    LET,

    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    COLON,

    // Operators
    ASSIGN,
    GREATER_THAN,
    LESS_THAN,
    PLUS,
    MINUS,
    ASTERISK,
    DIVISION,

    NEWLINE,

    UNKNOWN,

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
void Lexer_init(Lexer*, const char* source);
static char Lexer_peek_char(Lexer* lexer) {}
static char Lexer_advance(Lexer* lexer) {}
static void skip_whitespace(Lexer* lexer) {}
static Token make_token(Lexer* lexer, TokenType type) {}
Token Lexer_next(Lexer* lexer) {}
Token Lexer_peek(Lexer* lexer) {}
static Token identifier(Lexer* lexer) {}
static Token number(Lexer* lexer) {}


#endif