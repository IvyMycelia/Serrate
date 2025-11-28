#include "lexer.h"


// Set up the lexer with pointers into the source, reset line/column counters, and prepare to start tokenizing.
void Lexer_init(Lexer* lexer, const char* source) {
    lexer->start, lexer->current = *source;
    lexer->line = 1;
    lexer->column = 0;
}

// Look at the current character without consuming it.
static char Lexer_peek_char(Lexer* lexer) {

}

// Consume the current character and move the pointer forward. Also updates line/column tracking.
static char Lexer_advance(Lexer* lexer) {

}

// Ignore characters that are irrelevant to syntax (spaces, tabs, newlines, comments). This keeps the actual tokenizer clean and fast.
static void skip_whitespace(Lexer* lexer) {

}

// Build a token struct using the lexer's current start and current pointers. Minimal and cheap.
static Token make_token(Lexer* lexer, TokenType type) {

}

// The main driver.
// Skips whitespace —> sets start —> consumes chars —> figures out which token to emit —> returns it.
Token Lexer_next(Lexer* lexer) {

}

// Consume a run of [A-Za-z0-9_].
// Then decide if it’s a keyword or IDENTIFIER.
static Token identifier(Lexer* lexer) {

}

// Consume a sequence of digits and return an INTEGER token.
static Token number(Lexer* lexer) {

}