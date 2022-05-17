#pragma once

struct Expr;

void ui_clear(Expr* e, size_t button_index);
void ui_addchar(Expr* e, size_t button_index);
void ui_evaluate(Expr* e, size_t button_index);
void ui_backspace(Expr* e, size_t button_index);

static const char* BUTTON_STRS[] =
{
    "<-", "?", "C", "(", ")",
    "7", "8", "9", "/", "%",
    "4", "5", "6", "*", "^",
    "1", "2", "3", "-", "?",
    "0", " ", ".", "+", "=",
};
static void (* const BUTTON_FUNCS[])(Expr*, size_t) =
{
    ui_backspace,   0,              ui_clear,       ui_addchar,     ui_addchar,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     0,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,
    ui_addchar,     ui_addchar,     ui_addchar,     ui_addchar,     0,
    ui_addchar,     0,              ui_addchar,     ui_addchar,     ui_evaluate,
};
const size_t BUTTONS_SIZE = sizeof(BUTTON_STRS)/sizeof(BUTTON_STRS[0]);
const int BUTTONS_PER_ROW = 5;