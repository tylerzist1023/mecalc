#pragma once

const size_t EXPR_INPUT_CAPACITY = 64;
const size_t EXPR_CAPACITY = EXPR_INPUT_CAPACITY*4;
struct Expr
{
	char str[EXPR_CAPACITY];
	size_t cursor;
};

// enum TokenType
// {
// 	KIND_BRACKET,
// 	KIND_OPERATOR,
// 	KIND_OPERAND
// };
// struct Token
// {
// 	TokenType type;
// 	union
// 	{
// 		char c;
// 		double d;
// 		long long int i;
// 	};
// };

#ifdef DEBUG
void expr_tests();
#else
#define expr_tests(...)
#endif

void expr_clear(Expr* e);
void expr_input(Expr* e, char c);
void expr_append(Expr* e, char c);
void expr_backspace(Expr* e);
void expr_set(Expr* e, const char* buf);
double expr_evaluate(Expr* e);
char expr_last(Expr* e);

bool is_operand(char c);
bool is_operator(char c);