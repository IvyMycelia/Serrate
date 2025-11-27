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
    PRINT,

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

typedef enum CharClass {
    CHAR_ALPHA,
    CHAR_DIGIT,
    CHAR_WS,
    CHAR_NEWLINE,
    CHAR_SYMBOL,
    CHAR_OTHER
} CharClass;

typedef struct Lexer {
    const char* start;
    const char* current;
    int line, column;
} Lexer;


void Lexer_init(Lexer*, const char* source);
Token Lexer_next(Lexer*);
Token Lexer_peek(Lexer*);

#endif