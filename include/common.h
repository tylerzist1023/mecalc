// TODO: PCH
#pragma once

#include <stdio.h>
#include <assert.h>

#define NO_DEBUG_PRINT

#if defined(DEBUG) && !defined(NO_DEBUG_PRINT)
#define PRINT printf
#else
#define PRINT(...)
#endif