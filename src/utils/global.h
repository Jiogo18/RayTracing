#pragma once

#ifdef _ASSERT
#define ASSERT _ASSERT
#else
#define ASSERT __glibcxx_assert
#endif

#ifdef WIN32
#include <windef.h>
#else
typedef struct
{
    int cx;
    int cy;
} SIZE;
#endif