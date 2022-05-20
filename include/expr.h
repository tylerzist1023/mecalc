#pragma once
#include "common.h"

#ifdef DEBUG
void expr_tests();
#else
#define expr_tests(...)
#endif

enum TokenType
{
    TYPE_VACANT = 0,
    TYPE_BRACKET_OPEN,
    TYPE_BRACKET_CLOSE,
    TYPE_OPERATOR,
    TYPE_OPERAND_NUMBER,
    TYPE_OPERAND_CONSTANT, // pi, e, etc.
    TYPE_OPERAND_VARIABLE,

    TYPE_AMBIGUOUS, // Hyphens and plus signs are ambiguous, as they can either be a part of the number, or behave as an operator.
    /** If the character...
     * Is the first character in the string,
     * comes directly after another operator,
     * comes directly after an opening bracket,
     * or comes directly after an exponent symbol (E), 
     */
    // ...then the character is of type OPERAND, otherwise, it is of type OPERATOR
    // By the time the evaluation func will get to the expression, then no tokens should be ambiguous. If they are, then the function will return an error

    TYPE_UNKNOWN,
};

enum ExprFlags
{
    EXPR_FLAG_VARIABLES = 0b1,
    EXPR_FLAG_DECIMAL = 0b10
};

const size_t EXPR_CAPACITY = 64;
struct Token
{
    TokenType type;
    union
    {
        char c;
        double d;
        long long int i;
    };
};
struct Expr
{
    Token data[EXPR_CAPACITY];
    int size;
};

// TODO: I am not certain on whether this is a good idea. I think all I want is just raw string parsing.
// So, I may just need to bite the bullet and rewrite this a third time.
const size_t TOKEN_CAPACITY = 32;
struct StringToken
{
    size_t index;
    size_t next;
    size_t prev;
    char data[TOKEN_CAPACITY];
    TokenType type;
    size_t size;
};
struct ExprString
{
    size_t head;
    size_t tail;
    size_t next_vacant;

    // This corresponds to which node the user is currently editing.
    // It ended up being too annoying passing a cursor value on the outside, so I will just put it here
    size_t cursor;

    uint32_t flags;

    StringToken data[EXPR_CAPACITY];
};

void expr_clear(ExprString* s);
void expr_clear(Expr* e);
void expr_append(ExprString* s, char c);
void expr_backspace(ExprString* s);
double expr_evaluate(ExprString* s, Expr* e);
void expr_set(ExprString* s, const char* str);
void expr_print(ExprString* s);
void expr_get_str(ExprString* s, char* str);

TokenType get_token_type(char c);