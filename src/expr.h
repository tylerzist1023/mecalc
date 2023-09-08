#pragma once
#include "common.h"

enum TokenType
{
    TK_UNKNOWN,
    TK_N_VALUE,
    TK_N_VAR,
    TK_BRACKET_OPEN,
    TK_BRACKET_CLOSE,
    TK_OPERATOR
};

enum OperatorType
{
    /* binary operators */
    OP_ADD = 0,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,

    /* prefix unary operators */
    OP_POS,
    OP_NEG,
    OP_LN,
    OP_LOG10,
    OP_ARCSIN,
    OP_ARCCOS,
    OP_ARCTAN,
    OP_SINH,
    OP_COSH,
    OP_TANH,
    OP_SIN,
    OP_COS,
    OP_TAN,
    OP_ABS,
    OP_SQRT,

    /* postfix unary operators */
    OP_FAC,

    OP_NUL,

    OP_SIZE
};

enum ProgrammerOperatorType
{
    /* binary operators */
    OP_PROG_ADD = 0,
    OP_PROG_SUB,
    OP_PROG_MUL,
    OP_PROG_DIV,
    OP_PROG_MOD,
    /* bitwise operators */
    OP_PROG_AND,
    OP_PROG_OR,
    OP_PROG_XOR,
    OP_PROG_LSH,
    OP_PROG_RSH,
    OP_PROG_ROL,
    OP_PROG_ROR,

    /* prefix unary operators */
    OP_PROG_POS,
    OP_PROG_NEG,
    OP_PROG_ABS,
    /* bitwise operators */
    OP_PROG_NOT,

    OP_PROG_NUL,

    OP_PROG_SIZE
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
        ProgrammerOperatorType po;
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
void expr_evaluate_x(const char *str, double* x_vals, double* y_vals, size_t vals_count);
int64_t expr_programmer_evaluate(const char* str);

#ifdef DEBUG
void expr_print(Expr* e);
void expr_tests();
#else
#define expr_print(...)
#define expr_tests(...)
#endif

bool is_valid_token(char c);