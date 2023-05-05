#pragma once

#include <cstdint>
#include <Common/CString.h>
#include <StringView.h>
#include <Format.h>
#include <String.h>
#include <Vector.h>
#include <Utility.h>

namespace Neon {
namespace Logger {
void WriteLine(Cxxutil::StringView &Line) { for(auto Character: Line) Serial::PrintCharacter(Character); }
void WriteLine(Cxxutil::String &Line) { for(auto Character : Line) Serial::PrintCharacter(Character); }
void WriteLine(const char *Line) { for(int i = 0; i < Line; i++) Serial::PrintCharacter(Line[i]);  }
}

template<typename... Args>
inline void Log(const char *Format, const Args&... Arguments) {
	Logger::WriteLine(Cxxutil::Format(Format, Cxxutil::Forward<Args>(Arguments)...));
}

template<typename... Args>
inline void Warn(const char *Message, const Args&... Arguments) {
    Log("[WARN] {}{}", Message, Cxxutil::Forward<Args>(Arguments)...);
}

template<typename... Args>
inline void Error(const char *Message, const Args&... Arguments) {
	Log("[ERROR] {}{}", Message, Cxxutil::Forward<Args>(Arguments)...);
}
}
