#pragma once

#include <Core/Panic.h>

namespace Neon {
    extern "C" {
        #define AssertWithMessage(x, Message)
        #define AssertWithoutMessage(x) (!(x) ? AssertionFail("Assertion failure: " #x, __FILE__, __LINE__, __PRETTY_FUNCTION__)) : (void)(x)
        #define GetMacro(_1, _2, Name, ...) Name
        #define Assert(...) GetMacro(__VA_ARGS__, AsserWithMessage, AssertWithoutMessage)(__VA_ARGS__)
    }
}