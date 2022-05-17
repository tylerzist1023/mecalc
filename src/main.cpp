#define _CRT_SECURE_NO_WARNINGS
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

    Expr expr;
    expr_clear(&expr);

    while(!WindowShouldClose())
    {
        SW = GetScreenWidth(), SH = GetScreenHeight();

        bool input = false;
        int key;
        while(key = GetCharPressed())
        {
            if(is_operand(key) || is_operator(key) || key == '(' || key == ')')
            {
                expr_input(&expr, key);
            }
            if(key == 'c')
            {
                expr_clear(&expr);
            }
        }
        if(IsKeyPressed(KEY_ENTER))
        {
            ui_evaluate(&expr, 0);
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui_backspace(&expr, 0);
        }

        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
            GuiTextBox({2,2,(float)SW-2, 64}, expr.str, 20, false);

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
                        (*BUTTON_FUNCS[i])(&expr, i);
                    }
                }
            }
        }
        EndDrawing();
    }

    return 0;
}