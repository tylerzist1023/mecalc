#include "ui.h"
#include "expr.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

namespace ui
{
namespace normal
{

FUNC_DEF(clear)
{
    memset(str, 0, EXPR_CAPACITY);
}
FUNC_DEF(addchar)
{
    if(strlen(str) < EXPR_CAPACITY)
        str[strlen(str)] = BUTTON_STRS[button_index][0];
}
FUNC_DEF(evaluate)
{
    double result = expr_evaluate(str);

    if(result == result)
    {
        memset(str, 0, EXPR_CAPACITY);
        snprintf(str, EXPR_CAPACITY, "%G", result);

        // Extremely sus. For some reason %G always outputs a lower case e for scientific notation, and since we use E for that, change it.
        // Why not just use e, you may ask? Because we intend to use e for Euler's number
        size_t str_len = strlen(str);
        for(int i = 0; i < str_len; i++)
        {
            if(str[i] == 'e')
                str[i] = 'E';
        }
    }
    else
    {
        // TODO: Show the user that the input is invalid. Like make the background red or something
        //expr_clear(s);
    }
}
FUNC_DEF(backspace)
{
    if(*str != '\0')
        str[strlen(str)-1] = '\0';
}
FUNC_DEF(left)
{
}
FUNC_DEF(right)
{
}

};

namespace graph
{
void evaluate(char* str, double* x_vals, double* y_vals, size_t vals_count)
{
    for(size_t i = 0; i < vals_count; i++)
    {
        y_vals[i] = expr_evaluate_x(str, x_vals[i]);
    }
}
};

};