#include "expr.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stack>

// TODO: multi-character operators
/*
    log
    ln
    trig, arctrig, trigh,
    pi
*/
// TODO: Get rid of the const tokens in the expr after tokenization. Proving more work managing them than need be

#ifdef DEBUG
static void token_print(Token t)
{
    switch(t.type)
    {
        case TK_N_VALUE:
        case TK_N_CONST:
            PRINT("[%lf] ", t.d);
            break;
        default:
            PRINT("[%d %c] ", t.o, t.c);
            break;
    }
}
void expr_print(Expr* e)
{
    for(size_t i = 0; i < e->size; i++)
    {
        token_print(e->data[i]);
    }
    PRINT("\n");
}

static void expr_test(const char* str, double answer)
{
    double result = expr_evaluate(str);
    static uint32_t case_num = 0; case_num++;
    const double epsilon = 0.0001;

    if(result == result || answer == answer)
    {
        printf("Test case %u: %s == %lf ... %s (evaluated to %lf)\n",
            case_num,
            str,
            answer,
            abs(result - answer) <= epsilon ? "\x1b[32mPASSED\x1b[0m" : "\x1b[31mFAILED\x1b[0m",
            result);
    }
    else
    {
        printf("Test case %u: %s == %lf ... %s (evaluated to %lf)\n",
            case_num,
            str,
            answer,
            abs(result - answer) <= epsilon ? "\x1b[31mFAILED\x1b[0m" : "\x1b[32mPASSED\x1b[0m",
            result);
    }
}

void expr_tests()
{
    expr_test("2+2", 4);
    expr_test("2/2+1*(5*(1+4))", 26);
    expr_test("2*-1", -2);
    expr_test("2*(-1)", -2);
    expr_test("-1*2", -2);
    expr_test("3*-(2+-6)", 12);
    expr_test("(2+-6)", -4);
    expr_test("(-6)", -6);
    expr_test("5!", 120);
    expr_test("(2+5!)", 122);
    expr_test("2(-6)", -12);
    expr_test("-2(6)", -12);
    expr_test("(-2)6", -12);
    expr_test("e^2+1e", 10.10734);
    expr_test("4^2(e)", 43.4925092553);
    expr_test("4^(2)(e)", 43.4925092553);
    expr_test("4^(2)e", 43.4925092553);
    expr_test("4^(e)2", 86.6161208536);
    expr_test("e^2-1ee", 0);

    // TODO: This does not work as intended. I would prefer to have this since this I type operations like this on my TI-84.
    // A hacky solution would be to do the classic check if the brackets are balanced, then just add the missing bracket tokens at the end of the Expr
    expr_test("2(6+1", 14); 
    expr_test("2(-6", -12);

    expr_test("2)6", 12);
    expr_test("-2)-6", -8);
    expr_test("--1", 1);
    expr_test("-+1", -1);
    expr_test("+-1", -1);


    /* old tests */
    expr_test("2+2", 4.0);
    expr_test("2-2", 0.0);
    expr_test("2+4*8", 34.0);
    expr_test("2*(((3-4)*4-5)+1)", -16.0);
    expr_test("1/10", 0.1);
    expr_test("5(6)", 30.0);
    expr_test("(6)5", 30.0);
    expr_test("-5", -5);
    expr_test("0", 0);
    expr_test("((((((((0))))))))", 0);
    expr_test("2^2^2", 16.0);
    expr_test("100*(-5)^2", 2500);
    expr_test("4+5+(-5*(-5/2))", 21.5);
    expr_test("-5^2", 25);
    expr_test("10*-5", -50);
    expr_test("(4)(2)", 8);
    expr_test("(-5*-10+-4^3-4*(5(8(9))))", -1454);
    expr_test("-10*+10", -100);
    expr_test("+10*-10", -100);
    expr_test("+10/+10", 1);
    expr_test("1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1", 32);
    expr_test("0.1+0.2", 0.3);
    expr_test("1.3*555.555", 722.2215);
    expr_test("1/3.1", 0.3225806);
    expr_test("1.5E2", 150);
    expr_test("1.5E+2", 150);
    expr_test("1.5E1+1E-1", 15.1);
    expr_test("2^.5", 1.4142135623730950488016887242097);
    expr_test("4^2(2)", 32);
    expr_test("4^(2)(2)", 32);
    expr_test("4^(2)2", 32);
    expr_test("ee", 7.38905609893);
    expr_test("-e", -M_E);
    expr_test("2*-p", 2*-M_PI);

    /* fail cases */
    expr_test("-1^.5", NAN);
    expr_test("(1/0)*2+(3^7)", NAN);
    expr_test("E", 0);
    expr_test("(", NAN);
    expr_test("43009***93420***20+", NAN);
    expr_test("20++", NAN);
}
#endif

bool is_valid_token(char c)
{
    switch(c)
    {
        case '(':
        case ')':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
        case 'E':
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '!':
        case 'e':
        case 'p':
        case 'x':
            return true;
        default:
            return false;
    }
}

static inline bool isdigit(char c)
{
    return c >= '0' && c <= '9' || c == '.' || c == 'E';
}
static inline bool isconstant(char c)
{
    return c == 'e' || c == 'p';
}
static inline bool isvariable(char c)
{
    return c == 'x';
}
static inline Token token_default()
{
    return {.type = TK_UNKNOWN, .c = '\0', .i = 0};
}
static inline double factorial(double x)
{
    return tgamma(x+1);
}
static inline double negate(double x)
{
    return -x;
}

static Token scan_token(const char* str, const char** next, 
    bool* unary_prefix, bool* unary_postfix)
{
    Token t = token_default();
    const char* _next = str;
    if(isdigit(*str) || isconstant(*str) || isvariable(*str))
    {
        if(isconstant(*str))
        {
            switch(*str)
            {
                case 'e':
                    t.type = TK_N_CONST;
                    t.d = M_E;
                    break;
                case 'p':
                    t.type = TK_N_CONST;
                    t.d = M_PI;
                    break;
            }
            _next++;
        }
        else if(isdigit(*str))
        {
            t.type = TK_N_VALUE;
            t.d = strtod(str, (char**)&_next);
        }
        else if(isvariable(*str))
        {
            t.type = TK_N_VAR;
            t.c = *str;
            _next++;
        }
        *unary_postfix = true;
        *unary_prefix = false;
    }
    else 
    {
        t.c = *str;
        switch(*str)
        {
            case '(':
                t.type = TK_BRACKET_OPEN;
                *unary_prefix = true;
                break;
            case ')':
                t.type = TK_BRACKET_CLOSE;
                break;

            case '+':
                t.type = TK_OPERATOR;
                if(*unary_prefix) //TODO: broken
                {
                    t.o = OP_POS;
                    break;
                }
                t.o = OP_ADD;
                *unary_prefix = true;
                break;
            case '-':
                t.type = TK_OPERATOR;
                if(*unary_prefix)
                {
                    t.o = OP_NEG;
                    break;
                }
                t.o = OP_SUB;
                *unary_prefix = true;
                break;
            case '*':
                t.type = TK_OPERATOR;
                t.o = OP_MUL;
                *unary_prefix = true;
                break;
            case '/':
                t.type = TK_OPERATOR;
                t.o = OP_DIV;
                *unary_prefix = true;
                break;
            case '^':
                t.type = TK_OPERATOR;
                t.o = OP_POW;
                *unary_prefix = true;
                break;

            case '!':
                t.type = TK_OPERATOR;
                t.o = OP_FAC;
                break;

            default:
                t.type = TK_UNKNOWN;
                // TODO: multi-width token parsing
                break;
        }
        _next++;

        *unary_postfix = false;
    }

    if(next) *next = _next;
    return t;
}

static void expr_tokenize(const char* str, Expr* e)
{
    bool unary_prefix = true;
    bool unary_postfix = false;
    for(const char* c = str; (*c != '\0') && (e->size < EXPR_CAPACITY);)
    {
        const char* next;
        Token t = scan_token(c, &next, &unary_prefix, &unary_postfix);
        c = next;

        e->data[e->size++] = t;
    }
}

void expr_clear(Expr* e)
{
    e->size = 0;
    for(size_t i = 0; i < EXPR_CAPACITY; i++)
    {
        e->data[i] = token_default();
    }
}

static inline int get_arg_count(OperatorType o)
{
    switch(o)
    {
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_POW:
            return 2;
        case OP_POS:
        case OP_NEG:
        case OP_FAC:
            return 1;
        default:
            return 1;
    }
}

static void evaluate_op(std::stack<Token>& ops, std::stack<Token>& values)
{
    if(values.empty() || ops.empty())
    {
        while(!ops.empty()) ops.pop();
        while(!values.empty()) values.pop();
        values.push({.type = TK_N_VALUE, .d = NAN});
        return;
    }
    // if(ops.top().type != TK_OPERATOR)
    // {
    //     ops.pop();
    //     return;
    // }

    int arg_count = get_arg_count(ops.top().o);
    if(arg_count == 1)
    {
        Token val1 = values.top(); values.pop();

        switch(ops.top().o)
        {
            case OP_POS:
                break;
            case OP_NEG:
                PRINT("Negated %lf\n", val1.d);
                val1.d = -val1.d;
                break;
            case OP_FAC:
                val1.d = (double)factorial(val1.d);
                break;
            default:
                break;
        }
        values.push(val1);
    }
    else if(arg_count == 2)
    {
        Token val2 = values.top(); values.pop();

        if(values.empty() || ops.empty())
        {
            while(!ops.empty()) ops.pop();
            while(!values.empty()) values.pop();
            values.push({.type = TK_N_VALUE, .d = NAN});
            return;
        }

        Token val1 = values.top(); values.pop();

        double result = 0.0;

        switch(ops.top().o)
        {
            case OP_ADD:
                result = val1.d + val2.d;
                break;
            case OP_SUB:
                result = val1.d - val2.d;
                break;
            case OP_MUL:
                result = val1.d * val2.d;
                break;
            case OP_DIV:
                if(val2.d == 0)
                    result = NAN;
                else
                    result = val1.d / val2.d;
                break;
            case OP_POW:
                result = pow(val1.d, val2.d);
                break;
            default:
                break;
        }
        values.push({.type = TK_N_VALUE, .d = result});
    }
    ops.pop();
}

static inline int precedence(OperatorType o)
{
    switch(o)
    {
        case OP_ADD:
        case OP_SUB:
            return 1;
        case OP_MUL:
        case OP_DIV:
            return 2;
        case OP_POW:
            return 3;
        case OP_POS:
        case OP_NEG:
            return 4;
        case OP_FAC:
            return 5;
        default:
            return 0;
    }
    return 0;
}

// TODO: still some bugs
static double expr_evaluate(Expr* e)
{
    // TODO: Highly illegal. Make a custom data structure instead of using this trash
    std::stack<Token> ops;
    std::stack<Token> values;

    for(size_t i = 0; i < e->size; i++)
    {
        switch(e->data[i].type)
        {
            case TK_BRACKET_OPEN:
                if(i != 0 && (e->data[i-1].type == TK_N_VALUE || e->data[i-1].type == TK_N_CONST))
                {
                    while(!ops.empty() && precedence(OP_MUL) < precedence(ops.top().o) && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_op(ops, values);
                    }
                    ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                }
                else if(i > 1 && e->data[i-1].type == TK_BRACKET_CLOSE && (e->data[i-2].type == TK_N_VALUE || e->data[i-2].type == TK_N_CONST))
                {
                    while(!ops.empty() && precedence(OP_MUL) < precedence(ops.top().o) && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_op(ops, values);
                    }
                    ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                }
                ops.push(e->data[i]);
                break;
            case TK_BRACKET_CLOSE:
                while(!ops.empty() && ops.top().type != TK_BRACKET_OPEN)
                {
                    evaluate_op(ops, values);
                }
                if(!ops.empty()) ops.pop();
                break;

            case TK_N_VALUE:
                if(i != 0 && (e->data[i-1].type == TK_BRACKET_CLOSE || e->data[i-1].type == TK_N_CONST))
                {
                    if(e->data[i-1].type == TK_N_CONST)
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                    }
                    while(!ops.empty() && precedence(OP_MUL) < precedence(ops.top().o) && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_op(ops, values);
                    }
                    if(e->data[i-1].type == TK_BRACKET_CLOSE) 
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                    }
                }
                else
                {
                    values.push(e->data[i]);
                }
                break;
            case TK_N_CONST:
                if(i != 0 && (e->data[i-1].type == TK_BRACKET_CLOSE || e->data[i-1].type == TK_N_CONST || e->data[i-1].type == TK_N_VALUE))
                {
                    if(e->data[i-1].type == TK_N_CONST || e->data[i-1].type == TK_N_VALUE)
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                    }
                    while(!ops.empty() && precedence(OP_MUL) < precedence(ops.top().o) && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_op(ops, values);
                    }
                    if(e->data[i-1].type == TK_BRACKET_CLOSE) 
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                    }
                }
                else
                {
                    values.push(e->data[i]);
                }
                break;

            case TK_OPERATOR:
                while(!ops.empty() && ops.top().type != TK_BRACKET_OPEN && precedence(e->data[i].o) < precedence(ops.top().o))
                {
                    evaluate_op(ops, values);
                }
                ops.push(e->data[i]);
                break;

            default:
                return NAN;
                break;
        }
    }

    if(values.empty()) return NAN;
    while(!ops.empty() && !values.empty())
    {
        evaluate_op(ops, values);
    }
    return values.top().d;
}

// TODO: It is not a good idea to re-tokenize everything each time we call the function.
// We should have a way of only tokenizing the string when we need to.
double expr_evaluate_x(const char *str, double x)
{
    Expr e;
    expr_clear(&e);
    expr_tokenize(str, &e);

    for(size_t i = 0; i < e.size; i++)
    {
        if(e.data[i].type == TK_N_VAR && e.data[i].c == 'x')
        {
            e.data[i].type = TK_N_CONST;
            e.data[i].d = x;
        }
    }

    expr_print(&e);

    return expr_evaluate(&e);
}

double expr_evaluate(const char* str)
{
    Expr e;
    expr_clear(&e);
    expr_tokenize(str, &e);
    expr_print(&e);

    return expr_evaluate(&e);
}