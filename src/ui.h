#pragma once
#include "common.h"
#include "expr.h"

namespace ui
{
enum Mode
{
    MODE_NORMAL,
    MODE_PROGRAMMER,
    MODE_GRAPH,
};

namespace normal
{

#define NORMAL_BUTTON_FUNC_DEF(name) void cb##name(char* str, size_t button_index)
NORMAL_BUTTON_FUNC_DEF(clear);
NORMAL_BUTTON_FUNC_DEF(addchar);
NORMAL_BUTTON_FUNC_DEF(evaluate);
NORMAL_BUTTON_FUNC_DEF(backspace);
NORMAL_BUTTON_FUNC_DEF(left);
NORMAL_BUTTON_FUNC_DEF(right);

static const char* BUTTON_STRS[] =
{
    "<-", "?", "C", "(", ")",
    "7", "8", "9", "/", "%",
    "4", "5", "6", "*", "^",
    "1", "2", "3", "-", "!",
    "0", " ", ".", "+", "=",
};
static void (* const BUTTON_FUNCS[])(char*, size_t) =
{
    cbbackspace,    0,                      cbclear,    cbaddchar,  cbaddchar,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  0,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  cbaddchar,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  cbaddchar,
    cbaddchar,      0,                      cbaddchar,  cbaddchar,  cbevaluate,
};
const size_t BUTTONS_SIZE = sizeof(BUTTON_STRS)/sizeof(BUTTON_STRS[0]);
const int BUTTONS_PER_ROW = 5;

};

namespace programmer
{
enum Base
{
    BASE_BINARY = 0,
    BASE_OCTAL,
    BASE_DECIMAL,
    BASE_HEXADECIMAL,
    BASE_SIZE,
};

#define PROGRAMMER_BUTTON_FUNC_DEF(name) void cb##name(char strs[][EXPR_CAPACITY], Base selected, size_t button_index)
PROGRAMMER_BUTTON_FUNC_DEF(clear);
PROGRAMMER_BUTTON_FUNC_DEF(addchar);
PROGRAMMER_BUTTON_FUNC_DEF(evaluate);
PROGRAMMER_BUTTON_FUNC_DEF(backspace);
PROGRAMMER_BUTTON_FUNC_DEF(left);
PROGRAMMER_BUTTON_FUNC_DEF(right);

static const char* BUTTON_STRS[] =
{
    "<-", "C", "?","rol","ror",
    "D", "E", "F", "<<", ">>",
    "A", "B", "C", "%", "^",
    "7", "8", "9", "/", "|",
    "4", "5", "6", "*", "&",
    "1", "2", "3", "-", "~",
    "0", "(", ")", "+", "=",
};
static void (* const BUTTON_FUNCS[])(char[][EXPR_CAPACITY], Base, size_t) =
{
    cbbackspace, cbclear, 0, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbaddchar,
    cbaddchar, cbaddchar, cbaddchar, cbaddchar, cbevaluate,
};
const size_t BUTTONS_SIZE = sizeof(BUTTON_STRS)/sizeof(BUTTON_STRS[0]);
const int BUTTONS_PER_ROW = 5;
const int BUTTONS_ROWS = BUTTONS_SIZE/BUTTONS_PER_ROW;
};

namespace graph
{
void evaluate(char* str, double* x_vals, double* y_vals, size_t vals_count);

struct ScreenCoord
{
    int x,y;
};
struct ScreenBounds
{
    int x1,y1,x2,y2;
};
struct GraphCoord
{
    double x,y;
};
struct GraphBounds
{
    double x1,y1,x2,y2;
};

bool operator==(ScreenBounds lhs, ScreenBounds rhs);
bool operator!=(ScreenBounds lhs, ScreenBounds rhs);

double screen_to_graph_x(ScreenBounds sb, GraphBounds gb, int sx);
double screen_to_graph_y(ScreenBounds sb, GraphBounds gb, int sy);
int graph_to_screen_x(ScreenBounds sb, GraphBounds gb, double gx);
int graph_to_screen_y(ScreenBounds sb, GraphBounds gb, double gy);
GraphCoord screen_to_graph(ScreenBounds sb, GraphBounds gb, ScreenCoord sc);
ScreenCoord graph_to_screen(ScreenBounds sb, GraphBounds gb, GraphCoord gc);
bool screen_contains(ScreenBounds sb, ScreenCoord sc);
GraphBounds adjust_graph_bounds(ScreenBounds sb, GraphBounds gb);

inline int clamp(int min, int x, int max)
{
    assert(max >= min);

    if(x <= min)
        return min;
    else if(x >= max)
        return max;
    return x;
}
};
};