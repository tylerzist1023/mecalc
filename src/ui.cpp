#include "ui.h"
#include "expr.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

UI_FUNC(clear)
{
    expr_clear(s);
}
UI_FUNC(addchar)
{
    // TODO: this only appends the first character of the button string
    expr_append(s, BUTTON_STRS[button_index][0]);
}
UI_FUNC(evaluate)
{
    Expr expr;
    double result = expr_evaluate(s, &expr);

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
        expr_clear(s);
        expr_set(s, buf);
    }
    else
    {
        // TODO: Show the user that the input is invalid. Like make the background red or something
        //expr_clear(s);
    }
}
UI_FUNC(backspace)
{
    expr_backspace(s);
}
UI_FUNC(left)
{
    if(s->cursor != s->head)
        s->cursor = s->data[s->cursor].prev;
}
UI_FUNC(right)
{
    if(s->cursor != s->tail)
        s->cursor = s->data[s->cursor].next;
}