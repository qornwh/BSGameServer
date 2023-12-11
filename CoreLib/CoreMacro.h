#pragma once


/*----------
:   CRASH
------------*/

#define CRASH(cause)                \
{                                   \
    volatile int* crash = nullptr;  \
    *crash = 0xDEADBEEF;            \
    exit(1);                        \
}

#define ASSERT_CRASH(expr)          \
{                                   \
    if (!(expr))                    \
    {                               \
        CRASH("ASSERT_CRASH");      \
    }                               \
}