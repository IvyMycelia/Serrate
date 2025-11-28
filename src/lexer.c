#include <string.h>

#include "lexer.h"

/* 
    Helper Functions 
*/
// Increment line and reset column
inline void increment_newline(Lexer* lexer) {
    lexer->column = 0; lexer->line++;
}

// Check if the character is a unicode latin alphabet character
static inline int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

// Check if the character is a unicode arabic numeral character
static inline int is_digit(char c) {
    return (c >= 0) && (c <= 9);
}


/*
    Start and Current allow the Lexer to return the full token. For example, 
    the start of a keyword 'func' would be 'f', and the current 
    (after looping through the keyword) would be 'c', which will let us know 
    the full token is 'func'.

    Line: Starts at 1, because that's the beginning of the file, duh
    Column: Starts at 0 in Serrate
*/
// Set up the lexer with pointers into the source, reset line/column counters, and prepare to start tokenizing.
void Lexer_init(Lexer* lexer, const char* source) {
    lexer->start = source;      // Set start to current character
    lexer->current = source;    // Set current to current character
    lexer->line = 1;            // Set line to 1
    lexer->column = 0;          // Set column to 0
}


/*
    If the next character is '\0', aka EndOfFile, then return '\0'. If not, 
    then return whatever the next character is.
*/
// Look at the current character without consuming it.
static char Lexer_peek_char(Lexer* lexer) {
    if (*lexer->current== '\0') return '\0';    // Return '\0' if EndOfFile
    return *lexer->current;                     // Return next character if not
}

// Consume the current character and move the pointer forward. Also updates line/column tracking.
static char Lexer_advance(Lexer* lexer) {
    char c = *lexer->current++; // Set C to next current character
    switch (c) {                // Check if C is a newline.
        case '\n': increment_newline(lexer);
        case '\r': increment_newline(lexer);
        // case '\r\n': increment_newline(lexer);
        // case '\n\r': increment_newline(lexer);
        // case '\036 ': increment_newline(lexer);
        // case '\025 ': increment_newline(lexer);

        default: lexer->column++;   // If not a newline, increment column by 1
    }
    return c;   // Return C
}

// Ignore characters that are irrelevant to syntax (spaces, tabs, newlines, comments). This keeps the actual tokenizer clean and fast.
static void skip_whitespace(Lexer* lexer) {
    for (;;) {
        char c = Lexer_peek_char(lexer);
        switch (c) {
            case ' ':   // Space
            case '\t':  // Tab
            case '\v':  // Vertical Tab
            case '\f':  // Form Feed (Page Break)
            case '\r':  // Carriage Return
                Lexer_advance(lexer);
                break;
            case '\n':  // Newline
                Lexer_advance(lexer);
                break;
            case '#':   // Comment
                while (Lexer_peek_char(lexer) != '\n' && Lexer_peek_char(lexer) != '\0') Lexer_advance(lexer);
                break;
            default: return;
        }
    }
}

// Build a token struct using the lexer's current start and current pointers. Minimal and cheap.
static Token make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.length = (int)(lexer->current - lexer->start);
    token.type = type;              // One of the Enum values of TokenType
    token.start = lexer->start;     // The beginning character of the token
    token.line = lexer->line;       // What line the token is on
    token.column = lexer->column;   // What column the token is on
    return token;
}

// The main driver.
// Skips whitespace —> sets start —> consumes chars —> figures out which token to emit —> returns it.
Token Lexer_next(Lexer* lexer) {
    skip_whitespace(lexer);         // Skip any whitespace characters
    lexer->start = lexer->current;  // Reset start to current

    char c = Lexer_advance(lexer);    // Set character to current

    if (c == '\0') return make_token(lexer, TOKEN_EOF); // If EndOfFile, return EOF Token
    if (is_alpha(c)) return identifier(lexer);          // If an alphabet character, return identifier
    if (is_digit(c)) return number(lexer);              // If a numeric character, return it's numerical value

    // Single-character tokens (minimal punctuation and operators)
    switch (c) {
        // Delimiters
        case '(': return make_token(lexer, LEFT_PAREN);
        case ')': return make_token(lexer, RIGHT_PAREN);
        case ':': return make_token(lexer, COLON);

        // Operators
        case '=': return make_token(lexer, ASSIGN);
        case '>': return make_token(lexer, GREATER_THAN);
        case '<': return make_token(lexer, LESS_THAN);

        case '+': return make_token(lexer, PLUS);
        case '-': return make_token(lexer, MINUS);
        case '*': return make_token(lexer, ASTERISK);
        case '/': return make_token(lexer, DIVISION);

        // Other
        case '\n': return make_token(lexer, NEWLINE);
        default: return make_token(lexer, UNKNOWN);
    }
}

// Consume a run of [A-Z, a-z, 0-9, _].
// Then decide if it’s a keyword or IDENTIFIER.
static Token identifier(Lexer* lexer) {
    // Keep advancing while he current character is alphanumeric or underscore
    while (is_alpha(Lexer_peek_char(lexer)) || is_digit(Lexer_peek_char(lexer))) Lexer_advance(lexer);

    int length = (int)(lexer->current - lexer->start);

    // Check for keywords (bare minimum)
    switch (lexer->start[0]) {
        case 'f': 
            if (length == 4 &&
                lexer->start[1] == 'u' &&
                lexer->start[2] == 'n' &&
                lexer->start[3] == 'c'
            ) return make_token(lexer, FUNC); break;

        case 'i': 
            if (length == 2 &&
                lexer->start[1] == 'f'
            ) return make_token(lexer, IF);

        case 'w': 
            if (length == 5 &&
                lexer->start[1] == 'h' &&
                lexer->start[2] == 'i' &&
                lexer->start[3] == 'l' &&
                lexer->start[4] == 'e'
            ) return make_token(lexer, WHILE);

        case 'r': 
            if (length == 6 &&
                lexer->start[1] == 'e' &&
                lexer->start[2] == 't' &&
                lexer->start[3] == 'u' &&
                lexer->start[4] == 'r' &&
                lexer->start[5] == 'n'
            ) return make_token(lexer, RETURN);

        case 'e': 
            if (length == 3 &&
                lexer->start[1] == 'n' &&
                lexer->start[2] == 'd'
            ) return make_token(lexer, END);

        case 'l': 
            if (length == 3 &&
                lexer->start[1] == 'e' &&
                lexer->start[2] == 't'
            ) return make_token(lexer, LET);
        
        default: return make_token(lexer, IDENTIFIER);
    }

    return make_token(lexer, UNKNOWN);
}

// Consume a sequence of digits and return an INTEGER token.
static Token number(Lexer* lexer) {
    while (is_digit(Lexer_peek_char(lexer))) Lexer_advance(lexer);
    return make_token(lexer, INTEGER);
}

// Convert Token Type to a string for debugging
const char* token_type_name(TokenType type) {
    switch (type) {
        case IDENTIFIER:    return "IDENTIFIER";
        case INTEGER:       return "INTEGER";

        // Keywords
        case FUNC:          return "FUNC";
        case IF:            return "IF";
        case WHILE:         return "WHILE";
        case RETURN:        return "RETURN";
        case END:           return "END";
        case LET:           return "LET";

        // Delimiters
        case LEFT_PAREN:    return "LEFT_PAREN";
        case RIGHT_PAREN:   return "RIGHT_PAREN";
        case COLON:         return "COLON";

        // Operators
        case ASSIGN:        return "ASSIGN";
        case GREATER_THAN:  return "GREATER_THAN";
        case LESS_THAN:     return "LESS_THAN";
        case PLUS:          return "PLUS";
        case MINUS:         return "MINUS";
        case ASTERISK:      return "ASTERISK";
        case DIVISION:      return "DIVISION";

        // Other
        case NEWLINE:       return "NEWLINE";
        case UNKNOWN:       return "UNKNOWN";
        case TOKEN_EOF:     return "TOKEN_EOF";

        // Ruh Roh
        default:            return "HOW TF DID YOU GET HERE";
    }
}
