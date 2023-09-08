#include "ui.h"
#include "expr.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

namespace ui
{
namespace normal
{

NORMAL_BUTTON_FUNC_DEF(clear)
{
    memset(str, 0, EXPR_CAPACITY);
}
NORMAL_BUTTON_FUNC_DEF(addchar)
{
    size_t str_len = strlen(str);
    if(str_len < EXPR_CAPACITY)
        str[str_len] = BUTTON_STRS[button_index][0];
}
NORMAL_BUTTON_FUNC_DEF(evaluate)
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
NORMAL_BUTTON_FUNC_DEF(backspace)
{
    if(*str != '\0')
        str[strlen(str)-1] = '\0';
}
NORMAL_BUTTON_FUNC_DEF(left)
{
}
NORMAL_BUTTON_FUNC_DEF(right)
{
}

};

namespace programmer
{
PROGRAMMER_BUTTON_FUNC_DEF(clear)
{
    memset(strs[selected], 0, EXPR_CAPACITY);
}
PROGRAMMER_BUTTON_FUNC_DEF(addchar)
{
    size_t str_len = strlen(strs[selected]);
    if(str_len < EXPR_CAPACITY)
        strs[selected][str_len] = BUTTON_STRS[button_index][0];
}
PROGRAMMER_BUTTON_FUNC_DEF(evaluate)
{
    int64_t result = expr_programmer_evaluate(strs[selected]);

    static const char* BASE_FORMATS[] =
    {
        "%lld", // TODO: support formatting to binary
        "%o",
        "%lld",
        "%llX",
    };

    for(size_t i = 0; i < BASE_SIZE; i++)
    {
        memset(strs[i], 0, EXPR_CAPACITY);
        snprintf(strs[i], EXPR_CAPACITY, BASE_FORMATS[i], result);
    }
}
PROGRAMMER_BUTTON_FUNC_DEF(backspace)
{
    if(*(strs[selected]) != '\0')
        strs[selected][strlen(strs[selected])-1] = '\0';
}
PROGRAMMER_BUTTON_FUNC_DEF(left){}
PROGRAMMER_BUTTON_FUNC_DEF(right){}
};

namespace graph
{
void evaluate(char* str, double* x_vals, double* y_vals, size_t vals_count)
{
    expr_evaluate_x(str, x_vals, y_vals, vals_count);
}

bool operator==(ScreenBounds lhs, ScreenBounds rhs)
{
    return lhs.x1 == rhs.x1 &&
        lhs.x2 == rhs.x2 &&
        lhs.y1 == rhs.y1 &&
        lhs.y2 == rhs.y2;
}
bool operator!=(ScreenBounds lhs, ScreenBounds rhs)
{
    return !(lhs == rhs);
}

static inline int max(int a, int b)
{
    return a > b ? a : b;
}
static inline int min(int a, int b)
{
    return a < b ? a : b;
}

double screen_to_graph_x(ScreenBounds sb, GraphBounds gb, int sx)
{
    int sw = (sb.x2 - sb.x1);
    sx = (sx - sb.x1);
    double ratiox = sx / (double)sw;

    double gw = (gb.x2 - gb.x1);
    double gx = gw*ratiox;

    return gx + gb.x1;
}
double screen_to_graph_y(ScreenBounds sb, GraphBounds gb, int sy)
{
    int sh = (sb.y2 - sb.y1);
    sy = (sy - sb.y1);
    double ratioy = sy / (double)sh;

    double gh = (gb.y2 - gb.y1);
    double gy = gh*ratioy;

    return gy + gb.y1;
}
int graph_to_screen_x(ScreenBounds sb, GraphBounds gb, double gx)
{
    double gw = (gb.x2 - gb.x1);
    gx = (gx - gb.x1);
    double ratiox = gx / gw;

    int sw = (sb.x2 - sb.x1);
    int sx = sw*ratiox;

    return sx + sb.x1;
}
int graph_to_screen_y(ScreenBounds sb, GraphBounds gb, double gy)
{
    double gh = (gb.y2 - gb.y1);
    gy = (gy - gb.y1);
    double ratioy = gy / gh;

    int sh = (sb.y2 - sb.y1);
    int sy = sh*ratioy;

    return sy + sb.y1;
}
GraphCoord screen_to_graph(ScreenBounds sb, GraphBounds gb, ScreenCoord sc)
{
    return {screen_to_graph_x(sb, gb, sc.x), screen_to_graph_y(sb, gb, sc.y)};
}
ScreenCoord graph_to_screen(ScreenBounds sb, GraphBounds gb, GraphCoord gc)
{
    return {graph_to_screen_x(sb, gb, gc.x), graph_to_screen_y(sb, gb, gc.y)};
}
bool screen_contains(ScreenBounds sb, ScreenCoord sc)
{
    bool contains_x = sc.x >= sb.x1 && sc.x <= sb.x2;
    bool contains_y = sc.y >= sb.y1 && sc.y <= sb.y2;
    return contains_x && contains_y;
}
GraphBounds adjust_graph_bounds(ScreenBounds sb, GraphBounds gb)
{
    int maxb = max(sb.x2 - sb.x1, sb.y2 - sb.y1);
    int minb = min(sb.x2 - sb.x1, sb.y2 - sb.y1);

    double ratio = minb/(double)maxb;
    if(maxb == sb.x2 - sb.x1) // horizontal component is the bigger one
    {
        double ming = gb.y1 - gb.y2;

        double new_ming = ming*ratio;
        gb.y2 += (ming - new_ming)/2.0;
        gb.y1 -= (ming - new_ming)/2.0;
    }
    else // else the vertical component is the bigger one
    {
        double ming = gb.x2 - gb.x1;

        double new_ming = ming*ratio;
        gb.x1 += (ming - new_ming)/2.0;
        gb.x2 -= (ming - new_ming)/2.0;
    }

    return gb;
}

};

};