#define RAYGUI_IMPLEMENTATION
#include "common.h"
#include "raylib.h"
#include "raygui.h"
#include "expr.h"
#include "ui.h"

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
                case 'c':
                    ui::normal::cbclear(expr_str, 0);
                    break;
                default:
                    if(is_valid_token(key))
                    {
                        if(strlen(expr_str) < EXPR_CAPACITY)
                            expr_str[strlen(expr_str)] = key;
                    }
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
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
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

        if(IsKeyPressed(KEY_C))
        {
            ui::normal::cbclear(expr_str, 0);
        }
    }
    return true;
}

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

// TODO: create functions for x and y components separately
static GraphCoord screen_to_graph(ScreenBounds sb, GraphBounds gb, ScreenCoord sc)
{
    int sw = (sb.x2 - sb.x1);
    int sx = (sc.x - sb.x1);
    double ratiox = sx / (double)sw;
    int sh = (sb.y2 - sb.y1);
    int sy = (sc.y - sb.y1);
    double ratioy = sy / (double)sh;

    double gw = (gb.x2 - gb.x1);
    double gx = gw*ratiox;
    double gh = (gb.y2 - gb.y1);
    double gy = gh*ratioy;

    return {gx + gb.x1, gy + gb.y1};
}
static ScreenCoord graph_to_screen(ScreenBounds sb, GraphBounds gb, GraphCoord gc)
{
    double gw = (gb.x2 - gb.x1);
    double gx = (gc.x - gb.x1);
    double ratiox = gx / gw;
    double gh = (gb.y2 - gb.y1);
    double gy = (gc.y - gb.y1);
    double ratioy = gy / gh;

    int sw = (sb.x2 - sb.x1);
    int sx = sw*ratiox;
    int sh = (sb.y2 - sb.y1);
    int sy = sh*ratioy;

    return {sx + sb.x1, sy + sb.y1};
}
static bool screen_contains(ScreenBounds sb, ScreenCoord sc)
{
    bool contains_x = sc.x >= sb.x1 && sc.x <= sb.x2;
    bool contains_y = sc.y >= sb.y1 && sc.y <= sb.y2;
    return contains_x && contains_y;
}

static bool mode_graph()
{
    char expr_str[EXPR_CAPACITY];
    memset(expr_str, 0, EXPR_CAPACITY);

    ScreenBounds sb = {0, 64, SW, SH};
    GraphBounds gb = {-10, -10, 10, 10}; // TODO: movement controls

    double* y_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*y_vals));
    double* x_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*x_vals));
    for(size_t i = 0; i < sb.x2 - sb.x1; i++)
    {
        x_vals[i] = screen_to_graph(sb, gb, {(int)i+sb.x1, 0}).x;
    }

    while(mode == ui::MODE_GRAPH)
    {
        SW = GetScreenWidth(), SH = GetScreenHeight();
        ScreenBounds sb_new = {0, 64, SW, SH};
        if(sb != sb_new)
        {
            free(y_vals);
            free(x_vals);
            sb = {0, 64, SW, SH};
            y_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*y_vals));
            x_vals = (double*)malloc((sb.x2 - sb.x1)*sizeof(*x_vals));
            for(size_t i = 0; i < sb.x2 - sb.x1; i++)
            {
                x_vals[i] = screen_to_graph(sb, gb, {(int)i+sb.x1, 0}).x;
            }
            ui::graph::evaluate(expr_str, x_vals, y_vals, (sb.x2 - sb.x1));
        }

        int key;
        while((key = GetCharPressed()))
        {
            switch(key)
            {
                case 'c':
                    ui::normal::cbclear(expr_str, 0);
                    break;
                default:
                    if(is_valid_token(key))
                    {
                        if(strlen(expr_str) < EXPR_CAPACITY)
                            expr_str[strlen(expr_str)] = key;
                    }
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
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
            GuiSetStyle(DEFAULT, TEXT_SPACING, 2);

            GuiTextBox({2,2,(float)SW-2, 64}, expr_str, 20, false);

            for(int i = sb.x1; i < sb.x2; i++)
            {   
                ScreenCoord sc = graph_to_screen(sb, gb, {0, y_vals[i-sb.x1]});

                if(screen_contains(sb, sc))
                    DrawCircle(i, sc.y , 1.f, RAYWHITE);
            }
        }
        EndDrawing();
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
            case ui::MODE_GRAPH:
                interrupted = !mode_graph();
                break;
        }
    }

    return 0;
}