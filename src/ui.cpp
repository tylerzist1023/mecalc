#include "ui.h"
#include "expr.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void ui_clear(Expr* e, size_t button_index)
{
	expr_clear(e);
}
void ui_addchar(Expr* e, size_t button_index)
{
	// TODO: this only appends the first character of the button string
	expr_append(e, BUTTON_STRS[button_index][0]);
}
void ui_evaluate(Expr* e, size_t button_index)
{
	double result = expr_evaluate(e);

    if(result == result)
    {
        char buf[EXPR_CAPACITY];
        memset(buf, 0, EXPR_CAPACITY);

        snprintf(buf, EXPR_CAPACITY, "%G", result);

        // Extremely sus. For some reason %G always outputs a lower case e for scientific notation, and since we use E for that, change it.
        // Why not just use e, you may ask? Because we intend to use e for Euler's number
        size_t buf_len = strlen(buf);
        for(int i = 0; i < buf_len; i++)
        {
        	if(buf[i] == 'e')
        		buf[i] = 'E';
        }
        expr_clear(e);
        expr_set(e, buf);
    }
    else
    {
        expr_clear(e);
        expr_set(e, "ERR");
    }
}
void ui_backspace(Expr* e, size_t button_index)
{
	expr_backspace(e);
}