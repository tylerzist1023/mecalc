#pragma once

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

    TYPE_AMBIGUOUS, // Hyphens and plus signs are ambiguous, as they can either be a part of the number, or an operator.
    /** If the character...
     * Is the first character in the string,
     * comes directly after another operator,
     * or comes directly after an exponent symbol (E), 
     */
    // ...then the character is of type OPERAND, otherwise, it is of type OPERATOR.
    // In the first case, however, we can't tell that the character is of type OPERAND_NUMBER or of type OPERAND_CONSTANT,
    // because the user could type pi or e, or any numerical value and the token would still technically be valid.
    // By the time the evaluation func will get to the expression, then no tokens should be ambiguous. If they are, then the function will return an error

    TYPE_UNKNOWN,
};

const size_t EXPR_INPUT_CAPACITY = 64;
const size_t EXPR_CAPACITY = EXPR_INPUT_CAPACITY*4;
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

const size_t TOKEN_CAPACITY = 32;
struct StringToken
{
    size_t index;
    size_t next;
    size_t prev;
    char data[TOKEN_CAPACITY];
    TokenType type;
    int cursor;
};
struct ExprString
{
    size_t head;
    size_t tail;
    size_t next_vacant;
    StringToken data[EXPR_CAPACITY];
};

void expr_clear(ExprString* s);
void expr_clear(Expr* e);
void expr_append(ExprString* s, char c, size_t index);
size_t expr_backspace(ExprString* s, size_t index);
double expr_evaluate(ExprString* s, Expr* e);
void expr_set(ExprString* s, const char* str);
void expr_print(ExprString* s);
void expr_get_str(ExprString* s, char* str);

// TODO: I have no idea why I decided to expose this function. Should be cleaned up.
void token_clear(StringToken* s);

TokenType get_token_type(char c);