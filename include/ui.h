#pragma once

struct ExprString;

#define UI_FUNC(name) void ui_##name(ExprString* s, size_t button_index)

UI_FUNC(clear);
UI_FUNC(addchar);
UI_FUNC(evaluate);
UI_FUNC(backspace);
UI_FUNC(left);
UI_FUNC(right);

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
    ui_backspace,   0,              ui_clear,       ui_addchar,     ui_addchar,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     0,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     0,
    ui_addchar,     0,              ui_addchar,     ui_addchar,     ui_evaluate,
};
const size_t BUTTONS_SIZE = sizeof(BUTTON_STRS)/sizeof(BUTTON_STRS[0]);
const int BUTTONS_PER_ROW = 5;