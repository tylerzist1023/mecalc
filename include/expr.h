#pragma once
#include "common.h"

enum TokenType
{
    TK_UNKNOWN,
    TK_N_VALUE,
    TK_N_CONST,
    TK_N_VAR,
    TK_BRACKET_OPEN,
    TK_BRACKET_CLOSE,
    TK_OPERATOR
};

enum OperatorType
{
    /* binary operators */
    OP_ADD = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_DIV = 3,
    OP_POW = 4,

    /* unary operators */
    OP_POS = 5,
    OP_NEG = 6,
    OP_FAC = 7,

    OP_NUL
};

struct Token
{
    TokenType type;
    char c;
    union
    {
        double d;
        int64_t i;
        OperatorType o;
    };
};

const size_t EXPR_CAPACITY = 64;
struct Expr
{
    Token data[EXPR_CAPACITY];
    size_t size;
};

void expr_clear(Expr* e);
double expr_evaluate(const char* str);
double expr_evaluate_x(const char* str, double x);

#ifdef DEBUG
void expr_print(Expr* e);
void expr_tests();
#else
#define expr_print(...)
#define expr_tests(...)
#endif

bool is_valid_token(char c);