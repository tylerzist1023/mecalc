#include "expr.h"
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <stack>

enum OperatorOrg
{
    ORG_POST,
    ORG_IN,
    ORG_PRE
};

struct Operator
{
    const char* str;
    int arg_count;
    int precedence;
    OperatorOrg org;
    union
    {
    double (*func_d)(double, double);
    int64_t (*func_i)(int64_t, int64_t);
    };
} OPS[] = 
{
    {/* OP_ADD */       "+",        2, 1, ORG_IN,       [](double lhs, double rhs) -> double {return lhs+rhs;} },
    {/* OP_SUB */       "-",        2, 1, ORG_IN,       [](double lhs, double rhs) -> double {return lhs-rhs;} },
    {/* OP_MUL */       "*",        2, 2, ORG_IN,       [](double lhs, double rhs) -> double {return lhs*rhs;} },
    {/* OP_DIV */       "/",        2, 2, ORG_IN,       [](double lhs, double rhs) -> double {if(rhs == 0) return NAN; else return lhs/rhs;} },
    {/* OP_POW */       "^",        2, 3, ORG_IN,       [](double lhs, double rhs) -> double {return pow(lhs,rhs);} },     
    {/* OP_POS */       "+",        1, 4, ORG_PRE,      [](double x, double _) -> double {return x;} },
    {/* OP_NEG */       "-",        1, 4, ORG_PRE,      [](double x, double _) -> double {return -x;} },
    {/* OP_LN */        "ln",       1, 4, ORG_PRE,      [](double x, double _) -> double {return log(x);} },
    {/* OP_LOG10 */     "log",      1, 4, ORG_PRE,      [](double x, double _) -> double {return log10(x);} },
    {/* OP_ARCSIN */    "asin",     1, 4, ORG_PRE,      [](double x, double _) -> double {return asin(x);} },
    {/* OP_ARCCOS */    "acos",     1, 4, ORG_PRE,      [](double x, double _) -> double {return acos(x);} },
    {/* OP_ARCTAN */    "atan",     1, 4, ORG_PRE,      [](double x, double _) -> double {return atan(x);} },
    {/* OP_SINH */      "sinh",     1, 4, ORG_PRE,      [](double x, double _) -> double {return sinh(x);} },
    {/* OP_COSH */      "cosh",     1, 4, ORG_PRE,      [](double x, double _) -> double {return cosh(x);} },
    {/* OP_TANH */      "tanh",     1, 4, ORG_PRE,      [](double x, double _) -> double {return tanh(x);} },
    {/* OP_SIN */       "sin",      1, 4, ORG_PRE,      [](double x, double _) -> double {return sin(x);} },
    {/* OP_COS */       "cos",      1, 4, ORG_PRE,      [](double x, double _) -> double {return cos(x);} },
    {/* OP_TAN */       "tan",      1, 4, ORG_PRE,      [](double x, double _) -> double {return tan(x);} },
    {/* OP_ABS */       "abs",      1, 4, ORG_PRE,      [](double x, double _) -> double {return abs(x);} },
    {/* OP_SQRT */      "sqrt",     1, 4, ORG_PRE,      [](double x, double _) -> double {return sqrt(x);} },
    {/* OP_FAC */       "!",        1, 5, ORG_POST,     [](double x, double _) -> double {return tgamma(x+1);} },
    {/* OP_NUL */       "",         0, 0, ORG_IN,       0},
};

Operator OPS_PROGRAMMER[] =
{
    {/* OP_PROG_ADD */       "+",        2, 5, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs+rhs;} },
    {/* OP_PROG_SUB */       "-",        2, 5, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs-rhs;} },
    {/* OP_PROG_MUL */       "*",        2, 6, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs*rhs;} },
    {/* OP_PROG_DIV */       "/",        2, 6, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs/rhs;} },
    {/* OP_PROG_MOD */       "%",        2, 6, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs%rhs;} },
    
    {/* OP_PROG_AND */       "&",        2, 3, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs&rhs;} },
    {/* OP_PROG_OR */        "|",        2, 1, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs|rhs;} },
    {/* OP_PROG_XOR */       "^",        2, 2, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs^rhs;} },
    {/* OP_PROG_LSH */       "<<",       2, 4, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs<<rhs;} },
    {/* OP_PROG_RSH */       ">>",       2, 4, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return lhs>>rhs;} },
    {/* OP_PROG_ROL */       "rol",      2, 4, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return _rotl64(lhs, rhs);} },
    {/* OP_PROG_ROR */       "ror",      2, 4, ORG_IN,  .func_i=[](int64_t lhs, int64_t rhs) -> int64_t {return _rotr64(lhs, rhs);} },

    {/* OP_PROG_POS */       "+",        1, 8, ORG_PRE, .func_i=[](int64_t x, int64_t _) -> int64_t {return x;} },
    {/* OP_PROG_NEG */       "-",        1, 8, ORG_PRE, .func_i=[](int64_t x, int64_t _) -> int64_t {return -x;} },
    {/* OP_PROG_ABS */       "abs",      1, 8, ORG_PRE, .func_i=[](int64_t x, int64_t _) -> int64_t {return abs(x);} },
    
    {/* OP_PROG_NOT */       "~",        1, 8, ORG_PRE, .func_i=[](int64_t x, int64_t _) -> int64_t {return ~x;} },

    {/* OP_PROG_NUL */       "",         0, 0, ORG_IN,  0},
};

#ifdef DEBUG
static void token_print(Token t)
{
    switch(t.type)
    {
        case TK_N_VALUE:
            PRINT("[%lf] ", t.d);
            break;
        default:
            PRINT("[%d %c] ", t.o, t.c);
            break;
    }
}
static void token_programmer_print(Token t)
{
    switch(t.type)
    {
        case TK_N_VALUE:
            PRINT("[%lld] ", t.i);
            break;
        default:
            PRINT("[%d %c] ", t.po, t.c);
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
void expr_programmer_print(Expr* e)
{
    for(size_t i = 0; i < e->size; i++)
    {
        token_programmer_print(e->data[i]);
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
    expr_test("4^2e", 43.4925092553); // TODO: Need to think about this.
    expr_test("4^(2)(e)", 43.4925092553);
    expr_test("4^(2)e", 43.4925092553);
    expr_test("4^(e)2", 86.6161208536);
    expr_test("e^2-1ee", 0);
    expr_test("-lne", -1);

    // TODO: This does not work as intended. I would prefer to have this since this I type operations like this on my TI-84.
    // A hacky solution would be to do the classic check if the brackets are balanced, then just add the missing bracket tokens at the end of the Expr
    expr_test("2(6+1", 14); 
    expr_test("2(-6", -12);

    expr_test("2)6", 12);
    expr_test("-2)-6", -8);
    expr_test("--1", 1);
    expr_test("-+1", -1);
    expr_test("+-1", -1);
    expr_test("ln(1)", 0);
    expr_test("ln(e)", 1);
    expr_test("2ln(e)", 2);
    expr_test("sinh(0)", 0);

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

    // TODO: programmer mode test cases
}
#endif

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

static OperatorType scan_operator(const char* str, const char** next, OperatorOrg possible_org)
{
    for(size_t i = 0; i < OP_SIZE; i++)
    {
        const char* _next = str;
        if(possible_org == ORG_PRE && OPS[i].org != ORG_PRE)
        {
            continue;
        }
        else if(possible_org != ORG_POST && OPS[i].org == ORG_POST)
        {
            continue;
        }

        if(*str == OPS[i].str[0])
        {
            while(OPS[i].str[_next - str] != '\0' && *_next == OPS[i].str[_next - str])
            {
                _next++;
            }
            if(OPS[i].str[_next - str] == '\0')
            {
                if(next)
                {
                    *next = _next;
                }
                return (OperatorType)i;
            }
        }
    }
    return OP_NUL;
}
static ProgrammerOperatorType scan_programmer_operator(const char* str, const char** next, OperatorOrg possible_org)
{
    for(size_t i = 0; i < OP_PROG_SIZE; i++)
    {
        const char* _next = str;
        if(possible_org == ORG_PRE && OPS_PROGRAMMER[i].org != ORG_PRE)
        {
            continue;
        }
        else if(possible_org != ORG_POST && OPS_PROGRAMMER[i].org == ORG_POST)
        {
            continue;
        }

        if(*str == OPS_PROGRAMMER[i].str[0])
        {
            while(OPS_PROGRAMMER[i].str[_next - str] != '\0' && *_next == OPS_PROGRAMMER[i].str[_next - str])
            {
                _next++;
            }
            if(OPS_PROGRAMMER[i].str[_next - str] == '\0')
            {
                if(next)
                {
                    *next = _next;
                }
                return (ProgrammerOperatorType)i;
            }
        }
    }
    return OP_PROG_NUL;
}

static void expr_tokenize(const char* str, Expr* e)
{
    OperatorOrg possible_org = ORG_PRE;

    for(const char* c = str; (*c != '\0') && (e->size < EXPR_CAPACITY);)
    {
        const char* next = c;
        Token t = token_default();
        if(isdigit(*c) || isconstant(*c) || isvariable(*c))
        {
            if(isconstant(*c))
            {
                switch(*c)
                {
                    case 'e':
                        t.d = M_E;
                        break;
                    case 'p':
                        t.d = M_PI;
                        break;
                }
                t.type = TK_N_VALUE;
                next++;
            }
            else if(isdigit(*c))
            {
                t.type = TK_N_VALUE;
                t.d = strtod(c, (char**)&next);
            }
            else if(isvariable(*c))
            {
                t.type = TK_N_VAR;
                t.c = *c;
                next++;
            }
            possible_org = ORG_POST;
        }
        else 
        {
            t.c = *c;
            switch(*c)
            {
                case '(':
                    t.type = TK_BRACKET_OPEN;
                    possible_org = ORG_PRE;
                    next++;
                    break;
                case ')':
                    t.type = TK_BRACKET_CLOSE;
                    next++;
                    break;

                default:
                    t.o = scan_operator(c, &next, possible_org);
                    if(t.o != OP_NUL)
                    {
                        t.type = TK_OPERATOR;
                        if(OPS[t.o].org != ORG_POST)
                        {
                            possible_org = ORG_PRE;
                        }
                    }
                    else
                    {
                        t.type = TK_UNKNOWN;
                        next++;
                    }
                    break;
            }
        }

        c = next;
        e->data[e->size++] = t;
    }
}
static void expr_programmer_tokenize(const char* str, Expr* e)
{
    OperatorOrg possible_org = ORG_PRE;

    for(const char* c = str; (*c != '\0') && (e->size < EXPR_CAPACITY);)
    {
        const char* next = c;
        Token t = token_default();
        if(isdigit(*c) || isvariable(*c))
        {
            if(isdigit(*c))
            {
                t.type = TK_N_VALUE;
                t.i = _strtoi64(c, (char**)&next, 10);
            }
            else if(isvariable(*c))
            {
                t.type = TK_N_VAR;
                t.c = *c;
                next++;
            }
            possible_org = ORG_POST;
        }
        else 
        {
            t.c = *c;
            switch(*c)
            {
                case '(':
                    t.type = TK_BRACKET_OPEN;
                    possible_org = ORG_PRE;
                    next++;
                    break;
                case ')':
                    t.type = TK_BRACKET_CLOSE;
                    next++;
                    break;

                default:
                    t.po = scan_programmer_operator(c, &next, possible_org);
                    if(t.po != OP_PROG_NUL)
                    {
                        t.type = TK_OPERATOR;
                        if(OPS_PROGRAMMER[t.po].org != ORG_POST)
                        {
                            possible_org = ORG_PRE;
                        }
                    }
                    else
                    {
                        t.type = TK_UNKNOWN;
                        next++;
                    }
                    break;
            }
        }

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

static void evaluate_op(std::stack<Token>& ops, std::stack<Token>& values)
{
    if(values.empty() || ops.empty())
    {
        while(!ops.empty()) ops.pop();
        while(!values.empty()) values.pop();
        values.push({.type = TK_N_VALUE, .d = NAN});
        return;
    }

    int arg_count = OPS[ops.top().o].arg_count;
    if(arg_count == 1)
    {
        Token val1 = values.top(); values.pop();

        val1.d = OPS[ops.top().o].func_d(val1.d, 0);
        
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

        double result = OPS[ops.top().o].func_d(val1.d, val2.d);

        values.push({.type = TK_N_VALUE, .d = result});
    }
    ops.pop();
}
static void evaluate_programmer_op(std::stack<Token>& ops, std::stack<Token>& values)
{
    if(values.empty() || ops.empty())
    {
        while(!ops.empty()) ops.pop();
        while(!values.empty()) values.pop();
        values.push({.type = TK_N_VALUE, .i = 0});
        return;
    }

    int arg_count = OPS_PROGRAMMER[ops.top().po].arg_count;
    if(arg_count == 1)
    {
        Token val1 = values.top(); values.pop();

        val1.i = OPS_PROGRAMMER[ops.top().po].func_i(val1.i, 0);
        
        values.push(val1);
    }
    else if(arg_count == 2)
    {
        Token val2 = values.top(); values.pop();

        if(values.empty() || ops.empty())
        {
            while(!ops.empty()) ops.pop();
            while(!values.empty()) values.pop();
            values.push({.type = TK_N_VALUE, .i = 0});
            return;
        }

        Token val1 = values.top(); values.pop();

        int64_t result = OPS_PROGRAMMER[ops.top().po].func_i(val1.i, val2.i);

        values.push({.type = TK_N_VALUE, .i = result});
    }
    ops.pop();
}

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
                if(i != 0 && (e->data[i-1].type == TK_N_VALUE))
                {
                    while(!ops.empty() && OPS[OP_MUL].precedence < OPS[ops.top().o].precedence && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_op(ops, values);
                    }
                    ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                }
                else if(i > 1 && e->data[i-1].type == TK_BRACKET_CLOSE && e->data[i-2].type == TK_N_VALUE)
                {
                    while(!ops.empty() && OPS[OP_MUL].precedence < OPS[ops.top().o].precedence && ops.top().type == TK_OPERATOR)
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
                if(i != 0 && (e->data[i-1].type == TK_BRACKET_CLOSE || e->data[i-1].type == TK_N_VALUE))
                {
                    if(e->data[i-1].type == TK_N_VALUE)
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                    }
                    while(!ops.empty() && OPS[OP_MUL].precedence < OPS[ops.top().o].precedence && ops.top().type == TK_OPERATOR)
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
                if(i != 0 && e->data[i-1].type == TK_N_VALUE && OPS[e->data[i].o].org == ORG_PRE)
                {
                    ops.push({.type = TK_OPERATOR, .c = '*', .o = OP_MUL});
                }
                while(!ops.empty() && ops.top().type != TK_BRACKET_OPEN && OPS[e->data[i].o].precedence < OPS[ops.top().o].precedence)
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
static int64_t expr_programmer_evaluate(Expr* e)
{
    // TODO: Highly illegal. Make a custom data structure instead of using this trash
    std::stack<Token> ops;
    std::stack<Token> values;

    for(size_t i = 0; i < e->size; i++)
    {
        switch(e->data[i].type)
        {
            case TK_BRACKET_OPEN:
                if(i != 0 && (e->data[i-1].type == TK_N_VALUE))
                {
                    while(!ops.empty() && OPS_PROGRAMMER[OP_PROG_MUL].precedence < OPS_PROGRAMMER[ops.top().po].precedence && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_programmer_op(ops, values);
                    }
                    ops.push({.type = TK_OPERATOR, .c = '*', .po = OP_PROG_MUL});
                }
                else if(i > 1 && e->data[i-1].type == TK_BRACKET_CLOSE && e->data[i-2].type == TK_N_VALUE)
                {
                    while(!ops.empty() && OPS_PROGRAMMER[OP_PROG_MUL].precedence < OPS_PROGRAMMER[ops.top().po].precedence && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_programmer_op(ops, values);
                    }
                    ops.push({.type = TK_OPERATOR, .c = '*', .po = OP_PROG_MUL});
                }
                ops.push(e->data[i]);
                break;
            case TK_BRACKET_CLOSE:
                while(!ops.empty() && ops.top().type != TK_BRACKET_OPEN)
                {
                    evaluate_programmer_op(ops, values);
                }
                if(!ops.empty()) ops.pop();
                break;

            case TK_N_VALUE:
                if(i != 0 && (e->data[i-1].type == TK_BRACKET_CLOSE || e->data[i-1].type == TK_N_VALUE))
                {
                    if(e->data[i-1].type == TK_N_VALUE)
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .po = OP_PROG_MUL});
                    }
                    while(!ops.empty() && OPS_PROGRAMMER[OP_PROG_MUL].precedence < OPS_PROGRAMMER[ops.top().po].precedence && ops.top().type == TK_OPERATOR)
                    {
                        evaluate_programmer_op(ops, values);
                    }
                    if(e->data[i-1].type == TK_BRACKET_CLOSE) 
                    {
                        values.push(e->data[i]);
                        ops.push({.type = TK_OPERATOR, .c = '*', .po = OP_PROG_MUL});
                    }
                }
                else
                {
                    values.push(e->data[i]);
                }
                break;
            case TK_OPERATOR:
                if(i != 0 && e->data[i-1].type == TK_N_VALUE && OPS_PROGRAMMER[e->data[i].po].org == ORG_PRE)
                {
                    ops.push({.type = TK_OPERATOR, .c = '*', .po = OP_PROG_MUL});
                }
                while(!ops.empty() && ops.top().type != TK_BRACKET_OPEN && OPS_PROGRAMMER[e->data[i].po].precedence < OPS_PROGRAMMER[ops.top().po].precedence)
                {
                    evaluate_programmer_op(ops, values);
                }
                ops.push(e->data[i]);
                break;

            default:
                return 0;
                break;
        }
    }

    if(values.empty()) return 0;
    while(!ops.empty() && !values.empty())
    {
        evaluate_programmer_op(ops, values);
    }
    return values.top().i;
}

// TODO: It is a horrible idea to re-tokenize everything each time we call the function.
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
            e.data[i].type = TK_N_VALUE;
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
int64_t expr_programmer_evaluate(const char* str)
{
    Expr e;
    expr_clear(&e);
    expr_programmer_tokenize(str, &e);
    expr_programmer_print(&e);

    return expr_programmer_evaluate(&e);
}