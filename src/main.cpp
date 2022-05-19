#define RAYGUI_IMPLEMENTATION

#include "common.h"
#include "raylib.h"
#include "raygui.h"
#include "expr.h"
#include "ui.h"
#include <stdio.h>

#pragma comment(lib, "raylibdll")

int SW = 800, SH = 608;

int main(int argc, char* argv[])
{
    expr_tests();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 608, "MeCalculator");

    SetTargetFPS(60);

    ExprString expr_str;
    expr_clear(&expr_str);

    while(!WindowShouldClose())
    {
        SW = GetScreenWidth(), SH = GetScreenHeight();

        bool input = false;
        int key;
        while(key = GetCharPressed())
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
            ui_evaluate(&expr_str, 0);
        }
        if(IsKeyPressed(KEY_LEFT))
        {
            ui_left(&expr_str, 0);
            PRINT("Left, New cursor: %d\n", expr_str.cursor);
        }
        if(IsKeyPressed(KEY_RIGHT))
        {
            ui_right(&expr_str, 0);
            PRINT("Right, New cursor: %d\n", expr_str.cursor);
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui_backspace(&expr_str, 0);
        }

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
            for(int i = 0; i < BUTTONS_SIZE; i++)
            {
                if(GuiButton(
                    {
                        (float)((i%BUTTONS_PER_ROW)*(SW/BUTTONS_PER_ROW)),
                        (float)(68+(i/BUTTONS_PER_ROW)*((SH-68)/BUTTONS_PER_ROW)), 
                        (float)(SW/BUTTONS_PER_ROW), 
                        (float)((SH-68)/BUTTONS_PER_ROW)
                    }, 
                    BUTTON_STRS[i]
                ))
                {
                    if(BUTTON_FUNCS[i] != 0)
                    {
                        (*BUTTON_FUNCS[i])(&expr_str, i);
                    }
                }
            }
        }
        EndDrawing();
    }

    return 0;
}