#pragma once

#include "assert.h"

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
    case LogLevel::BREAK_IF_DEBUGGING:
        DebugBreak();
        return;
    case LogLevel::FATAL:
        DebugBreak();
        exit(1);
    default:
        assert(false);
        return;
    }
}

inline void debug_print(LogLevel lvl, const char* str) {
    OutputDebugStringA(str);
    action_log_level(lvl);
}

inline void debug_print(LogLevel lvl, const wchar_t* str) {
    OutputDebugStringW(str);
    action_log_level(lvl);
}
