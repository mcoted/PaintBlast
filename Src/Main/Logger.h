#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static void LogError(const char* errorFormat, ...)
{
    va_list args;
    va_start(args, errorFormat);
    vfprintf(stderr, errorFormat, args);
    va_end(args);
}

static void LogErrorAndAbort(const char* errorFormat, ...)
{
    va_list args;
    va_start(args, errorFormat);
    vfprintf(stderr, errorFormat, args);
    va_end(args);

    abort();
}