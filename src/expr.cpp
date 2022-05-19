#include "common.h"
#include "expr.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: factorial, roots, log, trig

static inline double abs(double x)
{
    if(x < 0.0)
        return x*-1;
    return x;
}

#ifdef DEBUG
static bool expr_test(const char* buf, double value)
{
    static int expr_case = 0;

    ExprString expr_str;
    expr_set(&expr_str, buf);
    Expr expr;
    expr_clear(&expr);
    double result = expr_evaluate(&expr_str, &expr);
    double epsilon = 0.001;

    expr_print(&expr_str);
    printf("Expr test case %d: %s == %lf", ++expr_case, buf, value);
    if(result == result)
        printf(" ... %s (evaluated to %lf)\n", (abs(result - value) <= epsilon) ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m", result);
    else
        printf(" ... %s (evaluated to %lf)\n", (abs(result - value) <= epsilon) ? "\033[31mFAILED\033[0m" : "\033[32mPASSED\033[0m", result);

    return abs(result - value) <= epsilon;
}

void expr_tests()
{
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

    // fail cases
    expr_test("-1^.5", NAN);
    expr_test("(1/0)*2+(3^7)", NAN);
    expr_test("E", 0);
    expr_test("(", NAN);
    expr_test("43009***93420***20+", NAN);
    expr_test("20++", NAN);

    // TODO: add stuff for append/backspace
}
#endif

void expr_print(ExprString* s)
{
    size_t si = s->head;
    while(si != s->tail)
    {
        PRINT("[%s] ", s->data[si].data);
        si = s->data[si].next;
    }
    if(s->data[si].type != TYPE_VACANT)
    {
        PRINT("[%s] ", s->data[si].data);
    }
    PRINT("\n");
}

void expr_get_str(ExprString* s, char* str)
{
    memset(str, 0, EXPR_CAPACITY * TOKEN_CAPACITY);

    size_t si = s->head;
    while(si != s->tail)
    {
        strncat(str, s->data[si].data, EXPR_CAPACITY * TOKEN_CAPACITY);
        si = s->data[si].next;
    }
    if(s->data[si].type != TYPE_VACANT)
    {
        strncat(str, s->data[si].data, EXPR_CAPACITY * TOKEN_CAPACITY);
    }
}

inline TokenType get_token_type(char c)
{
    switch(c)
    {
        case '(':
            return TYPE_BRACKET_OPEN;
        case ')':
            return TYPE_BRACKET_CLOSE;
        case '*':
        case '/':
        case '^':
            return TYPE_OPERATOR;
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
            return TYPE_OPERAND_NUMBER;
        case 'e':
        case 'p': // TODO: Hmmm, works for now, but later I may want to change this.
            return TYPE_OPERAND_CONSTANT;
        case '-':
        case '+':
            return TYPE_AMBIGUOUS;
    }
    return TYPE_VACANT;
}

inline bool check_if_type_is_insertable(TokenType c, TokenType t, TokenType pt)
{
    if((c == TYPE_OPERAND_NUMBER) && c == t)
        return true;
    else if(t == TYPE_AMBIGUOUS && (c == TYPE_OPERAND_CONSTANT || c == TYPE_OPERAND_NUMBER) && (pt == TYPE_UNKNOWN || pt == TYPE_BRACKET_OPEN || pt == TYPE_OPERATOR))
        return true;
    return false;
}

void token_clear(StringToken *s)
{
    s->next = 0;
    s->prev = 0;
    for(size_t i = 0; i < TOKEN_CAPACITY; i++)
    {
        s->data[i] = '\0';
    }
    s->size = 0;
    s->type = TYPE_VACANT;
}

void expr_clear(ExprString* s)
{
    s->head = 0;
    s->tail = 0;
    s->next_vacant = 1;
    s->cursor = 0;
    for(size_t i = 0; i < EXPR_CAPACITY; i++)
    {
        token_clear(&s->data[i]);
        s->data[i].index = i;
    }
}

// TODO: This is a bad and naive O(n) solution. Make it better!
static size_t find_next_vacant(ExprString* s, size_t from)
{
    for(size_t i = (from+1)%EXPR_CAPACITY; i != from; i = (i+1)%EXPR_CAPACITY)
    {
        if(s->data[i].type == TYPE_VACANT)
        {
            return i;
        }
    }
    return from;
}

static void token_append(StringToken* s, char c)
{
    if(s->size == TOKEN_CAPACITY-1)
        return;
    s->data[s->size++] = c;
}
void expr_append(ExprString* s, char c)
{
    TokenType type = get_token_type(c);
    PRINT("Appending: %c\n", c);

    // TODO: This can be a chain of else if, not nested elses (they are exactly the same thing although one may be more readable)
    if(s->data[s->head].type == TYPE_VACANT)
    {
        PRINT("Appended '%c' to head.\n", c);
        token_append(&s->data[s->head], c);
        s->data[s->head].type = type;
        s->cursor = s->head;
    }
    else if(s->data[s->cursor].type == TYPE_VACANT)
    {
        return;
    }
    else
    {   
        if(type == TYPE_UNKNOWN)
            return;

        if(check_if_type_is_insertable(type, s->data[s->cursor].type, s->cursor == s->head ? TYPE_UNKNOWN : s->data[s->data[s->cursor].prev].type) || (s->data[s->cursor].size != 0 && s->data[s->cursor].data[s->data[s->cursor].size-1] == 'E'))
        {
            // If the type of the character matches the type of the cursor index, just insert there
            token_append(&s->data[s->cursor], c);
            s->data[s->cursor].type = type;
        }
        else
        {
            if(s->cursor == s->tail)
            {
                // Check if the list is full
                if(s->data[s->next_vacant].type != TYPE_VACANT)
                    return;

                s->data[s->cursor].next = s->next_vacant;
                s->tail = s->next_vacant;
                s->data[s->tail].prev = s->cursor;
                s->next_vacant = find_next_vacant(s, s->next_vacant);

                token_append(&s->data[s->data[s->cursor].next], c);
                s->data[s->data[s->cursor].next].type = type;

                if(s->cursor != s->head && s->data[s->cursor].type == TYPE_AMBIGUOUS)
                {
                    s->data[s->cursor].type = TYPE_OPERATOR;
                }

                s->cursor = s->tail;
            }
            else
            {
                // If cursor is NOT the tail, check to see the char is insertable into the next node
                if(check_if_type_is_insertable(type, s->data[s->cursor].type, s->cursor == s->head ? TYPE_UNKNOWN : s->data[s->data[s->cursor].prev].type) || (s->data[s->cursor].size != 0 && s->data[s->cursor].data[s->data[s->cursor].size-1] == 'E'))
                {
                    // Insert the data into the next node
                    token_append(&s->data[s->data[s->cursor].next], c);
                    s->data[s->data[s->cursor].next].type = type;
                    if(s->cursor != s->head && s->data[s->cursor].type == TYPE_AMBIGUOUS)
                    {
                        s->data[s->cursor].type = TYPE_OPERATOR;
                    }

                    s->cursor = s->data[s->cursor].next;
                }
                else
                {
                    // Check if the list is full
                    if(s->data[s->next_vacant].type != TYPE_VACANT)
                        return;

                    // Insert a new node in-between cursor and cursor->next
                    s->data[s->next_vacant].next = s->data[s->cursor].next;
                    s->data[s->next_vacant].prev = s->cursor;
                    s->data[s->data[s->cursor].next].prev = s->next_vacant;
                    s->data[s->cursor].next = s->next_vacant;
                    s->next_vacant = find_next_vacant(s, s->next_vacant);

                    token_append(&s->data[s->data[s->cursor].next], c);
                    s->data[s->data[s->cursor].next].type = type;

                    s->cursor = s->data[s->cursor].next;
                }
            }
        }
    }
    PRINT("Append ending: head: %zu, tail: %zu\n", s->head, s->tail);
    expr_print(s);
}

static void token_backspace(StringToken* s)
{
    if(s->size == 0)
        return;
    s->data[--s->size] = '\0';
}
void expr_backspace(ExprString* s)
{
    if(s->data[s->head].type == TYPE_VACANT)
    {
        expr_clear(s);
        s->cursor = 0;
    }
    else
    {
        token_backspace(&s->data[s->cursor]);

        // If the node is empty after the deletion, we need to delete the node
        if(s->data[s->cursor].size == 0)
        {
            size_t cursor_old = s->cursor;
            if(s->cursor == s->head && s->cursor == s->tail)
            {
                expr_clear(s);
                s->cursor = 0;
            }
            else if(s->cursor == s->head)
            {
                s->head = s->data[s->head].next;
                s->data[s->head].prev = 0;

                s->cursor = s->head;
            }
            else if(s->cursor == s->tail)
            {
                s->tail = s->data[s->tail].prev;
                s->data[s->tail].next = 0;

                s->cursor = s->tail;
            }
            else
            {
                s->data[s->data[s->cursor].prev].next = s->data[s->cursor].next;
                s->data[s->data[s->cursor].next].prev = s->data[s->cursor].prev;

                // TODO: Fix bug when user deletes an operator between two numerical values, the values don't get catted together.
                // For example, take 1+1. When the user removes the plus, this becomes 11, but this evaluates to one! It should be eleven! 

                // TODO: Should test to see which one is actually more appropriate, since I just guessed here
                s->cursor = s->data[s->data[s->cursor].next].prev;
            }
            token_clear(&s->data[cursor_old]);
        }
    }
}

void expr_clear(Expr* e)
{
    e->size = 0;
    for(size_t i = 0; i < EXPR_CAPACITY; i++)
    {
        e->data[i].type = TYPE_VACANT;
        e->data[i].i = 0;
    }
}
static void expr_append(Expr* e, Token value)
{
    if(e->size == EXPR_CAPACITY-1)
        return;
    e->data[e->size++] = value;
}
static Token expr_backspace(Expr* e)
{
    if(e->size == 0)
        return {.type = TYPE_VACANT, .d = 0.0};
    Token value = e->data[e->size-1];
    e->data[--e->size].type = TYPE_VACANT;
    e->data[e->size].i = 0;
    return value;
}
static Token expr_top(Expr* e)
{
    if(e->size == 0)
        return {.type = TYPE_VACANT, .d = 0.0};
    return e->data[e->size-1];
}

static void str_to_expr(ExprString* s, Expr* e)
{
    size_t si = s->head;
    size_t ei = 0;
    while(si != s->tail)
    {
        e->data[ei].type = s->data[si].type;
        switch(s->data[si].type)
        {   
            case TYPE_BRACKET_OPEN:
            case TYPE_BRACKET_CLOSE:
            case TYPE_OPERATOR:
            case TYPE_OPERAND_CONSTANT:
                e->data[ei].c = s->data[si].data[0];
                break;
            case TYPE_OPERAND_NUMBER:
                e->data[ei].d = strtod(s->data[si].data, 0);
                break;
            default:
                // TODO: Help.
                break;
        }

        si = s->data[si].next;
        ei++;
    }
    // TODO: This is a copypasta. Needs to get cleaned up
    if(s->data[si].type != TYPE_VACANT)
    {
        e->data[ei].type = s->data[si].type;
        switch(s->data[si].type)
        {   
            case TYPE_BRACKET_OPEN:
            case TYPE_BRACKET_CLOSE:
            case TYPE_OPERATOR:
            case TYPE_OPERAND_CONSTANT:
                e->data[ei].c = s->data[si].data[0];
                break;
            case TYPE_OPERAND_NUMBER:
                e->data[ei].d = strtod(s->data[si].data, 0);
                break;
            default:
                // TODO: Help.
                break;
        }
    }
    e->size = ei+1;
}

static int prec(char c)
{
    if (c == '^')
        return 3;
    else if (c == '/' || c == '*')
        return 2;
    else if (c == '+' || c == '-')
        return 1;
    else
        return 0;
}
static double evaluate_op(Expr* values, Expr* ops)
{
    double val1 = expr_top(values).d; expr_backspace(values);
    double val2 = expr_top(values).d; expr_backspace(values);
    double result = 0.0;
    switch(expr_top(ops).c)
    {
        case '+':
            result = val2 + val1;
            break;
        case '-':
            result = val2 - val1;
            break;
        case '*':
            result = val2 * val1;
            break;
        case '/':
            if(val1 == 0)
                result = NAN;
            else
                result = val2 / val1;
            break;
        case '^':
            result = pow(val2, val1);
            break;
        default:
            result = 0.0;
            break;
    }
    PRINT("Operation: %lf %c %lf = %lf\n", val2, expr_top(ops).c, val1, result);
    expr_backspace(ops);
    Token result_token = {.type = TYPE_OPERAND_NUMBER, .d = result};
    expr_append(values, result_token);

    return result;
}
// TODO: These expression edge cases are starting to get out of hand. Should come up with a better way to deal with this instead of hard-coding. 
double expr_evaluate(ExprString* s, Expr* e)
{
    expr_clear(e);
    str_to_expr(s, e);

    Expr values;
    Expr ops;
    expr_clear(&values);
    expr_clear(&ops);

    for(int i = 0; i < e->size; i++)
    {
        PRINT("Eval: %c %lf\n", e->data[i].c, e->data[i].d);
        switch(e->data[i].type)
        {
            case TYPE_BRACKET_OPEN:
                if(i != 0 && (e->data[i-1].type == TYPE_OPERAND_NUMBER || e->data[i-1].type == TYPE_OPERAND_CONSTANT))
                {
                    while(ops.size != 0 && prec(expr_top(&ops).c) >= prec('*') && expr_top(&ops).type == TYPE_OPERATOR)
                    {
                        evaluate_op(&values, &ops);
                    }
                    expr_append(&ops, {.type = TYPE_OPERATOR, .c = '*'});
                }
                else if(i > 1 && e->data[i-1].type == TYPE_BRACKET_CLOSE && (e->data[i-2].type == TYPE_OPERAND_NUMBER || e->data[i-2].type == TYPE_OPERAND_CONSTANT))
                {
                    while(ops.size != 0 && prec(expr_top(&ops).c) >= prec('*') && expr_top(&ops).type == TYPE_OPERATOR)
                    {
                        evaluate_op(&values, &ops);
                    }
                    expr_append(&ops, {.type = TYPE_OPERATOR, .c = '*'});
                }
                expr_append(&ops, e->data[i]);
                break;
            case TYPE_BRACKET_CLOSE:
                while(ops.size != 0 && expr_top(&ops).type != TYPE_BRACKET_OPEN)
                {
                    evaluate_op(&values, &ops);
                }
                if(ops.size != 0)
                {
                    expr_backspace(&ops);
                }
                break;
            case TYPE_OPERAND_NUMBER:
                if(i != 0 && (e->data[i-1].type == TYPE_BRACKET_CLOSE || e->data[i-1].type == TYPE_OPERAND_CONSTANT))
                {
                    while(ops.size != 0 && prec(expr_top(&ops).c) >= prec('*') && expr_top(&ops).type == TYPE_OPERATOR)
                    {
                        evaluate_op(&values, &ops);
                    }
                    expr_append(&ops, {.type = TYPE_OPERATOR, .c = '*'});
                }
                expr_append(&values, e->data[i]);
                break;
            case TYPE_OPERAND_CONSTANT:
                if(i != 0 && (e->data[i-1].type == TYPE_BRACKET_CLOSE || e->data[i-1].type == TYPE_OPERAND_NUMBER || e->data[i-1].type == TYPE_OPERAND_CONSTANT))
                {
                    while(ops.size != 0 && prec(expr_top(&ops).c) >= prec('*') && expr_top(&ops).type == TYPE_OPERATOR)
                    {
                        evaluate_op(&values, &ops);
                    }
                    expr_append(&ops, {.type = TYPE_OPERATOR, .c = '*'});
                }
                double constant;
                switch(e->data[i].c)
                {
                    case 'e':
                        constant = M_E;
                        break;
                    case 'p':
                        constant = M_PI;
                        break;
                }

                expr_append(&values, {.type = TYPE_OPERAND_NUMBER, .d = constant});
                break;
            case TYPE_OPERATOR:
                while(ops.size != 0 && prec(expr_top(&ops).c) >= prec(e->data[i].c))
                {
                    evaluate_op(&values, &ops);
                }
                expr_append(&ops, e->data[i]);
                break;
            case TYPE_AMBIGUOUS:
                return NAN;
                break;
        }
    }
    while(ops.size != 0 && values.size >= 2)
    {
        evaluate_op(&values, &ops);
    }
    if(ops.size != 0)
    {
        return NAN;
    }
    return expr_top(&values).d;
}

void expr_set(ExprString* s, const char* str)
{
    expr_clear(s);
    for(const char* it = str; *it != '\0'; it++)
    {
        expr_append(s, *it);
    }
}