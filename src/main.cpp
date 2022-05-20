#define RAYGUI_IMPLEMENTATION

#include "common.h"
#include "raylib.h"
#include "raygui.h"
#include "expr.h"
#include "ui.h"
#include <stdio.h>

#pragma comment(lib, "raylibdll")

int SW = 800, SH = 608;
ui::Mode mode = ui::MODE_NORMAL;

static inline void update_mode()
{
    if(IsKeyPressed(KEY_F1))
    {
        mode = ui::MODE_NORMAL;
    }
    else if(IsKeyPressed(KEY_F2))
    {
        mode = ui::MODE_GRAPH;
    }
}

static bool mode_normal()
{
    ExprString expr_str;
    expr_str.flags = EXPR_FLAG_DECIMAL;
    expr_clear(&expr_str);

    while(mode == ui::MODE_NORMAL)
    {
        if(WindowShouldClose())
            return false;

        SW = GetScreenWidth(), SH = GetScreenHeight();

        int key;
        while((key = GetCharPressed()))
        {
            if(get_token_type(key) != TYPE_UNKNOWN)
            {
                expr_append(&expr_str, key);
            }
            else if(key == 'c')
            {
                expr_clear(&expr_str);
            }
        }
        if(IsKeyPressed(KEY_ENTER))
        {
            ui::normal::cbevaluate(&expr_str, 0);
        }
        if(IsKeyPressed(KEY_LEFT))
        {
            ui::normal::cbleft(&expr_str, 0);
            PRINT("Head: %zu, Tail: %zu, Left, New cursor: %zu\n", expr_str.head, expr_str.tail, expr_str.cursor);
        }
        if(IsKeyPressed(KEY_RIGHT))
        {
            ui::normal::cbright(&expr_str, 0);
            PRINT("Head: %zu, Tail: %zu, Right, New cursor: %zu\n", expr_str.head, expr_str.tail, expr_str.cursor);
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui::normal::cbbackspace(&expr_str, 0);
        }
        update_mode();

        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
            
            char expr_cstr[EXPR_CAPACITY * TOKEN_CAPACITY];
            expr_get_str(&expr_str, expr_cstr);
            GuiTextBox({2,2,(float)SW-2, 64}, expr_cstr, 20, false);

            // TODO: Show the token the user is currently editing
            for(int i = 0; i < ui::normal::BUTTONS_SIZE; i++)
            {
                if(GuiButton(
                    {
                        (float)((i%ui::normal::BUTTONS_PER_ROW)*(SW/ui::normal::BUTTONS_PER_ROW)),
                        (float)(68+(i/ui::normal::BUTTONS_PER_ROW)*((SH-68)/ui::normal::BUTTONS_PER_ROW)),
                        (float)(SW/ui::normal::BUTTONS_PER_ROW),
                        (float)((SH-68)/ui::normal::BUTTONS_PER_ROW)
                    }, 
                    ui::normal::BUTTON_STRS[i]
                ))
                {
                    if(ui::normal::BUTTON_FUNCS[i] != 0)
                    {
                        (*ui::normal::BUTTON_FUNCS[i])(&expr_str, i);
                    }
                }
            }
        }
        EndDrawing();
    }
    return true;
}

static bool mode_graph()
{
    ExprString expr_strs[10];
    for(size_t i = 0; i < 10; i++)
        expr_clear(&expr_strs[i]);

    while(mode == ui::MODE_GRAPH)
    {
        if(WindowShouldClose())
            return false;

        update_mode();

        SW = GetScreenWidth(), SH = GetScreenHeight();
        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
            
            // char expr_cstr[EXPR_CAPACITY * TOKEN_CAPACITY];
            // expr_get_str(&expr_str[0], expr_cstr);
            // GuiTextBox({2,2,(float)SW-2, 64}, expr_cstr, 20, false);

            GuiLabel({64,64,256,64}, "GRAPH MODE");
        }
        EndDrawing();
    }
    return true;
}

int main(int argc, char* argv[])
{
    expr_tests();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 608, "MeCalculator");

    SetTargetFPS(60);

    bool interrupted = false;
    while(!interrupted)
    {
        switch(mode)
        {
            case ui::MODE_NORMAL:
                interrupted = !mode_normal();
                break;
            case ui::MODE_GRAPH:
                interrupted = !mode_graph();
                break;
        }
    }

    return 0;
}