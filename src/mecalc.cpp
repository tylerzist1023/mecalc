#include "raylib.h"
#include "raygui.h"
#include "common.h"
#include "expr.h"
#include "ui.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
        mode = ui::MODE_PROGRAMMER;
    }
    else if(IsKeyPressed(KEY_F3))
    {
        mode = ui::MODE_GRAPH;
    }
}

static bool mode_normal()
{
    char expr_str[EXPR_CAPACITY];
    memset(expr_str, 0, EXPR_CAPACITY);

    while(mode == ui::MODE_NORMAL)
    {
        if(WindowShouldClose())
            return false;

        SW = GetScreenWidth(), SH = GetScreenHeight();

        int key;
        while((key = GetCharPressed()))
        {
            switch(key)
            {
                case 'C':
                    ui::normal::cbclear(expr_str, 0);
                    break;
                default:
                    if(strlen(expr_str) < EXPR_CAPACITY)
                        expr_str[strlen(expr_str)] = key;
                    break;
            }
        }

        if(IsKeyPressed(KEY_ENTER))
        {
            ui::normal::cbevaluate(expr_str, 0);
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui::normal::cbbackspace(expr_str, 0);
        }
        update_mode();

        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);

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
                        (*ui::normal::BUTTON_FUNCS[i])(expr_str, i);
                    }
                }
            }

            GuiTextBox({2,2,(float)SW-2, 64}, expr_str, 20, false);
        }
        EndDrawing();
    }
    return true;
}

static bool mode_programmer()
{
    char expr_strs[ui::programmer::BASE_SIZE][EXPR_CAPACITY];
    ui::programmer::cbclear(expr_strs, ui::programmer::BASE_DECIMAL, 0);

    while(mode == ui::MODE_PROGRAMMER)
    {
        if(WindowShouldClose())
            return false;

        SW = GetScreenWidth(), SH = GetScreenHeight();

        int key;
        while((key = GetCharPressed()))
        {
            switch(key)
            {
                // case 'C':
                //     ui::normal::cbclear(expr_str, 0);
                //     break;
                default:
                    size_t str_len = strlen(expr_strs[ui::programmer::BASE_DECIMAL]);
                    if(str_len < EXPR_CAPACITY)
                        expr_strs[ui::programmer::BASE_DECIMAL][str_len] = key;
                    break;
            }
        }

        if(IsKeyPressed(KEY_ENTER))
        {
            ui::programmer::cbevaluate(expr_strs, ui::programmer::BASE_DECIMAL, 0);
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui::programmer::cbbackspace(expr_strs, ui::programmer::BASE_DECIMAL, 0);
        }
        update_mode();

        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);

            // TODO: Show the token the user is currently editing
            for(int i = 0; i < ui::programmer::BUTTONS_SIZE; i++)
            {
                if(GuiButton(
                    {
                        (float)((i%ui::programmer::BUTTONS_PER_ROW)*(SW/ui::programmer::BUTTONS_PER_ROW)),
                        (float)(68+(i/ui::programmer::BUTTONS_PER_ROW)*((SH-68)/ui::programmer::BUTTONS_ROWS)),
                        (float)(SW/ui::programmer::BUTTONS_PER_ROW),
                        (float)((SH-68)/ui::programmer::BUTTONS_ROWS)
                    }, 
                    ui::programmer::BUTTON_STRS[i]
                ))
                {
                    if(ui::programmer::BUTTON_FUNCS[i] != 0)
                    {
                        (*ui::programmer::BUTTON_FUNCS[i])(expr_strs, ui::programmer::BASE_DECIMAL, i);
                    }
                }
            }

            GuiTextBox({2,2,(float)SW-2, 64}, expr_strs[ui::programmer::BASE_DECIMAL], 20, false);
        }
        EndDrawing();
    }
    return true;
}

using namespace ui::graph;

static bool mode_graph()
{
    char expr_str[EXPR_CAPACITY];
    memset(expr_str, 0, EXPR_CAPACITY);

    ScreenBounds sb = {0, 64, SW, SH};
    GraphBounds gb_before = {-10, 10, 10, -10}; // TODO: movement controls
    GraphBounds gb = adjust_graph_bounds(sb, gb_before);

    double* y_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*y_vals));
    double* x_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*x_vals));
    for(int i = 0; i < sb.x2 - sb.x1; i++)
    {
        x_vals[i] = screen_to_graph_x(sb, gb, (int)i+sb.x1);
        y_vals[i] = NAN;
    }

    ScreenCoord mouse_prev = {0,0};
    while(mode == ui::MODE_GRAPH)
    {
        ScreenCoord mouse = {(int)GetMousePosition().x, (int)GetMousePosition().y};

        SW = GetScreenWidth(), SH = GetScreenHeight();
        ScreenBounds sb_new = {0, 64, SW, SH};
        if(sb != sb_new) // handle resizing
        {
            free(y_vals);
            free(x_vals);
            sb = {0, 64, SW, SH};
            gb = adjust_graph_bounds(sb, gb_before);
            y_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*y_vals));
            x_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*x_vals));
            for(int i = 0; i < sb.x2 - sb.x1; i++)
            {
                x_vals[i] = screen_to_graph_x(sb, gb, i+sb.x1);
                y_vals[i] = NAN;
            }
            ui::graph::evaluate(expr_str, x_vals, y_vals, (sb.x2 - sb.x1));
        }

        int key;
        while((key = GetCharPressed()))
        {
            switch(key)
            {
                case 'C':
                    ui::normal::cbclear(expr_str, 0);
                    break;
                default:
                    if(strlen(expr_str) < EXPR_CAPACITY)
                        expr_str[strlen(expr_str)] = key;
                    break;
            }
        }
        if(IsKeyPressed(KEY_ENTER))
        {
            ui::graph::evaluate(expr_str, x_vals, y_vals, (sb.x2 - sb.x1));
        }
        if(IsKeyPressed(KEY_BACKSPACE))
        {
            ui::normal::cbbackspace(expr_str, 0);
        }
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            GraphCoord mouse_graph = screen_to_graph(sb, gb, mouse);
            GraphCoord mouse_prev_graph = screen_to_graph(sb, gb, mouse_prev);
            GraphCoord mouse_delta = {mouse_graph.x - mouse_prev_graph.x, mouse_graph.y - mouse_prev_graph.y};

            // TODO: fix bug that resets the coords when resizing the window

            gb.x1 -= mouse_delta.x;
            gb.x2 -= mouse_delta.x;
            gb.y1 -= mouse_delta.y;
            gb.y2 -= mouse_delta.y;

            for(int i = 0; i < sb.x2 - sb.x1; i++)
            {
                x_vals[i] = screen_to_graph_x(sb, gb, (int)i+sb.x1);
                y_vals[i] = NAN;
            }
            ui::graph::evaluate(expr_str, x_vals, y_vals, (sb.x2 - sb.x1));
        }

        if(WindowShouldClose())
        {
            free(y_vals);
            free(x_vals);
            return false;
        }

        update_mode();

        ClearBackground(BLACK);
        BeginDrawing();
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);

            GuiTextBox({2,2,(float)SW-2, 64}, expr_str, 20, false);

            if(screen_contains(sb, mouse))
            {
                GraphCoord mouse_graph = screen_to_graph(sb, gb, mouse);
                int sy = graph_to_screen_y(sb, gb, y_vals[mouse.x-sb.x1]);

                char buf[32];
                snprintf(buf, 32, "(%G, %G)", mouse_graph.x, y_vals[mouse.x-sb.x1]);

                DrawText(buf, mouse.x, sy, 20, RAYWHITE);
            }

            ScreenCoord top = graph_to_screen(sb, gb, {0, gb.y1});
            ScreenCoord bottom = graph_to_screen(sb, gb, {0, gb.y2});
            ScreenCoord left = graph_to_screen(sb, gb, {gb.x1, 0});
            ScreenCoord right = graph_to_screen(sb, gb, {gb.x2, 0});

            int step = round((gb.y1-gb.y2)/10) > round((gb.x2-gb.x1)/10) ? round((gb.y1-gb.y2)/10) : round((gb.x2-gb.x1)/10);
            assert(step > 0);
            for(int i = (int)(gb.y2) - (int)(gb.y2)%step; i <= (int)(gb.y1); i+=step)
            {
                int tmpy = graph_to_screen_y(sb, gb, (double)i);
                DrawLine(left.x, tmpy, right.x, tmpy, DARKGRAY);
            }
            for(int i = (int)(gb.x1) - (int)(gb.x1)%step; i <= (int)(gb.x2); i+=step)
            {
                int tmpx = graph_to_screen_x(sb, gb, (double)i);
                DrawLine(tmpx, top.y, tmpx, bottom.y, DARKGRAY);
            }

            DrawLine(top.x, top.y, bottom.x, bottom.y, GRAY);
            DrawLine(left.x, left.y, right.x, right.y, GRAY);

            int sy_last;
            for(int i = sb.x1; i < sb.x2; i++)
            {   
                int sy = graph_to_screen_y(sb, gb, y_vals[i-sb.x1]);

                // TODO: fix when two adjacent points' lines don't get drawn, even though the line falls within the screen coords
                if(i != sb.x1 && y_vals[i-sb.x1] == y_vals[i-sb.x1] && y_vals[i-sb.x1-1] == y_vals[i-sb.x1-1]
                    && ((y_vals[i-sb.x1] <= gb.y1 && y_vals[i-sb.x1] >= gb.y2) || (y_vals[i-sb.x1-1] <= gb.y1 && y_vals[i-sb.x1-1] >= gb.y2)))
                {
                    DrawLine(i, clamp(sb.y1, sy, sb.y2), i-1, clamp(sb.y1, sy_last, sb.y2), RAYWHITE);
                }

                sy_last = sy;
            }
        }
        EndDrawing();

        mouse_prev = mouse;
    }
    free(y_vals);
    free(x_vals);
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
            case ui::MODE_PROGRAMMER:
                interrupted = !mode_programmer();
                break;
            case ui::MODE_GRAPH:
                interrupted = !mode_graph();
                break;
        }
    }

    return 0;
}