#include "common.h"
#include "expr.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: more robust number parsing system in general, the current code is pretty janky
// TODO: factorial, roots, log, trig, constants like pi and e
// TODO: all failed cases output ERR to the calculator

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

	Expr expr;
	expr_set(&expr, buf);
	double result = expr_evaluate(&expr);
	double epsilon = 0.001;

	printf("Expr test case %d: %s == %lf", ++expr_case, buf, value);
	printf(" ... %s (evaluated to %lf)\n", abs(result - value) <= epsilon ? "\033[32mPASSED\033[0m" : "\033[31mFAILED\033[0m", result);

	return abs(result - value) <= epsilon;
}

void expr_tests()
{
	expr_test("2+2", 4.0);
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

	// fail cases (currently these do not pass, they just get evaluated to 0)
	expr_test("E", NAN);
	expr_test("M", NAN);
	expr_test("(", NAN);
	expr_test("43009***93420***20+", NAN);
}
#endif

inline bool is_operand(char c)
{
	switch(c)
	{
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
        case 'M': // minus
        case '.': // decimal point
        case 'E': // exponent
        	return true;
	}
	return false;
}

inline bool is_operator(char c)
{
	switch(c)
	{
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        	return true;
	}
	return false;
}

void expr_clear(Expr* e)
{
	e->cursor = 0;
	for(size_t i = 0; i < EXPR_CAPACITY; i++)
	{
		e->str[i] = '\0';
	}
}

void expr_input(Expr* e, char c)
{
	if(e->cursor == EXPR_INPUT_CAPACITY-1)
		return;
	e->str[e->cursor++] = c;
}

void expr_append(Expr* e, char c)
{
	if(e->cursor == EXPR_CAPACITY-1)
		return;
	e->str[e->cursor++] = c;
}

void expr_backspace(Expr* e)
{
	if(e->cursor == 0)
		return;
	e->str[--e->cursor] = '\0';
}

void expr_set(Expr* e, const char* buf)
{
	expr_clear(e);
	size_t buf_len = strlen(buf);
	e->cursor = buf_len < EXPR_CAPACITY ? buf_len : EXPR_CAPACITY;
	for(size_t i = 0; i < e->cursor; i++)
	{
		e->str[i] = buf[i];
	}
}

char expr_last(Expr* e)
{
	if(e->cursor == 0)
		return '\0';
	return e->str[e->cursor-1];
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

static void infix_to_postfix(Expr* inx, Expr* pox)
{
	expr_clear(pox);
	Expr stack; expr_clear(&stack);

	for(size_t i = 0; i < inx->cursor; i++)
	{
		char c = inx->str[i];

		PRINT("c: %c\n", c);
		if(is_operand(c))
		{
			if(i != 0 && inx->str[i-1] == ')')
			{
				expr_append(&stack, '*');
				expr_append(pox, ' ');
			}
			expr_append(pox, c);
		}
		else if(c == '(')
		{
			if(i != 0 && is_operand(inx->str[i-1]))
			{
				expr_append(&stack, '*');
				expr_append(pox, ' ');
			}
			else if(i > 1 && is_operand(inx->str[i-2]) && inx->str[i-1] == ')')
			{
				expr_append(&stack, '*');
				expr_append(pox, ' ');
			}
			expr_append(&stack, c);
		}
		else if(c == ')')
		{
			char top;
			while((top = expr_last(&stack)) != '(')
			{
				expr_append(pox, top);
				expr_backspace(&stack);

				if(stack.cursor == 0)
				{
					return;
				}
			}
			expr_backspace(&stack);
		}
		else if(is_operator(c))
		{	
			if(c == '-' && (i == 0 || is_operator(inx->str[i-1]) || inx->str[i-1] == '('))
			{
				expr_append(pox, ' ');
				expr_append(pox, 'M');
			}
			else if(c == '-' && (inx->str[i-1] == 'E'))
			{
				expr_append(pox, '-');
			}
			else if(c == '+' && (i == 0 || is_operator(inx->str[i-1]) || inx->str[i-1] == '('))
			{
				expr_append(pox, ' ');
			}
			else if(c == '+' && (inx->str[i-1] == 'E'))
			{

			}
			else
			{
				expr_append(pox, ' ');
				char top = expr_last(&stack);
				while(stack.cursor != 0 && prec(c) <= prec(top) && is_operator(top))
				{
					if(c == '^' && top == '^')
						break;
					else
					{
						expr_append(pox, top);
						expr_backspace(&stack);
					}

					top = expr_last(&stack);
				}

				expr_append(&stack, c);
			}	
		}
		else
		{
			// TODO: error
			expr_clear(pox);
			return;
		}
		PRINT("Stack: %s\n", stack.str);
		PRINT("Postfix: %s\n\n", pox->str);
	}

	while(stack.cursor != 0)
	{
		char top = expr_last(&stack);

		expr_append(pox, top);
		expr_backspace(&stack);
	}

	assert(strlen(pox->str) == pox->cursor);
}

struct EvalStack
{
	double data[EXPR_CAPACITY];
	int size;
};
static inline void eval_clear(EvalStack* s)
{
	s->size = 0;
	for(size_t i = 0; i < EXPR_CAPACITY; i++)
	{
		s->data[i] = 0.0;
	}
}
static inline void eval_push(EvalStack* s, double value)
{
	if(s->size == EXPR_CAPACITY-1)
		return;
	s->data[s->size++] = value;
}
static inline double eval_pop(EvalStack* s)
{
	if(s->size == 0)
		return 0.0;
	double value = s->data[s->size-1];
	s->data[--s->size] = 0.0;
	return value;
}
static inline double eval_top(EvalStack* s)
{
	if(s->size == 0)
		return 0.0;
	return s->data[s->size-1];
}
double expr_evaluate(Expr* e)
{
	Expr pox;
	infix_to_postfix(e, &pox);
	if(pox.cursor == 0)
	{
		return NAN;
	}

	EvalStack stack;
	eval_clear(&stack);

	for(size_t i = 0; i < pox.cursor; i++)
	{
		char c = pox.str[i];
		PRINT("%c\n", c);
		if(!is_operator(c))
		{
			if(c == ' ')
				continue;

			double value = 0.0;
			bool negate;
			if(negate = (c == 'M'))
			{
				c = pox.str[++i];
			}
			
			char* start = pox.str+i;
			char* end = 0;
			value = strtod(start, &end);
			if(end - start > 0)
				i += end - start - 1;

			if(negate) value *= -1;
			eval_push(&stack, (double)value);
		}

		if(is_operator(c))
		{
			double val1 = eval_pop(&stack);
			double val2 = eval_pop(&stack);

			PRINT("Evaluating: %f %c %f\n", val2, c, val1);

			switch(c)
			{
				case '+': eval_push(&stack, val2+val1); break;
				case '-': eval_push(&stack, val2-val1); break;
				case '*': eval_push(&stack, val2*val1); break;
				case '/': eval_push(&stack, val2/val1); break;
				case '^': eval_push(&stack, pow(val2, val1)); break;
			}
		}
	}

	PRINT("Postfix str: %s\n", pox.str);

	double result = (double)eval_pop(&stack);
	PRINT("Result: %f\n", result);

	return result;
}