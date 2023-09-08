#ifdef PLATFORM_WIN32
#pragma comment(lib, "raylib")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "WinMM")
#pragma comment(lib, "User32")
#pragma comment(lib, "shell32")
#endif

#include "common.h"
#include "expr.h"
#include "ui.h"

#include "mecalc.cpp"
#include "expr.cpp"
#include "ui.cpp"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"