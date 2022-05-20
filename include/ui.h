#pragma once

struct ExprString;

#define FUNC_DEF(name) void cb##name(ExprString* s, size_t button_index)

namespace ui
{
enum Mode
{
    MODE_NORMAL,
    MODE_GRAPH
};

namespace normal
{

FUNC_DEF(clear);
FUNC_DEF(addchar);
FUNC_DEF(evaluate);
FUNC_DEF(backspace);
FUNC_DEF(left);
FUNC_DEF(right);

static const char* BUTTON_STRS[] =
{
    "<-", "?", "C", "(", ")",
    "7", "8", "9", "/", "%",
    "4", "5", "6", "*", "^",
    "1", "2", "3", "-", "?",
    "0", " ", ".", "+", "=",
};
static void (* const BUTTON_FUNCS[])(ExprString*, size_t) =
{
    cbbackspace,    0,                      cbclear,    cbaddchar,  cbaddchar,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  0,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  cbaddchar,
    cbaddchar,      cbaddchar,  cbaddchar,  cbaddchar,  0,
    cbaddchar,      0,                      cbaddchar,  cbaddchar,  cbevaluate,
};
const size_t BUTTONS_SIZE = sizeof(BUTTON_STRS)/sizeof(BUTTON_STRS[0]);
const int BUTTONS_PER_ROW = 5;

};
};