// TODO: PCH
#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define NO_DEBUG_PRINT

#if defined(DEBUG) && !defined(NO_DEBUG_PRINT)
#define PRINT printf
#else
#define PRINT(...)
#endif