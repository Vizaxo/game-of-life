#pragma once

#include "common.h"

#include <iostream>
#include <ostream>

enum class LogLevel {
    PRINT,
    BREAK_IF_DEBUGGING,
    FATAL,
};

inline void action_log_level(LogLevel lvl) {
    switch (lvl) {
    case LogLevel::PRINT:
        return;
#ifdef _TEST
    case LogLevel::BREAK_IF_DEBUGGING:
    case LogLevel::FATAL:
        extern int fatal_log_hit;
        ++fatal_log_hit;
        return;
#else
    case LogLevel::BREAK_IF_DEBUGGING:
        DebugBreak();
        return;
    case LogLevel::FATAL:
        DebugBreak();
        exit(1);
#endif
    default:
        assert(false);
        return;
    }
}

inline void debug_print(LogLevel lvl, const char* str) {
    OutputDebugStringA(str);
    OutputDebugStringA("\n");
    action_log_level(lvl);
}

inline void debug_print(LogLevel lvl, const wchar_t* str) {
    OutputDebugStringW(str);
    OutputDebugStringW(L"\n");
    action_log_level(lvl);
}

#define HRASSERT(hr) do{\
    if (FAILED(hr)) {\
        char buf[1024];\
        snprintf(buf, 1024, "HRESULT failed with code %x", hr);\
        debug_print(LogLevel::BREAK_IF_DEBUGGING, buf);\
    }\
}while(0)
