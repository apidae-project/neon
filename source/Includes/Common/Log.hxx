#pragma once

namespace Log {
    void Print(const char *Format, ...);
    void Info(const char Message);
    void Warn(const char Message);
    void Error(const char Message);
}