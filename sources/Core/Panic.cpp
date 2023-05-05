#include <Common/Log.h>
#include <Core/Panic.h>
#include <TypeInfo.h>
#include <Utility.h>

namespace Neon {
template<class... Args> [[noreturn]] void Panic(const char *File,
                        const int Line,
                        const char *Function,
                        const char *Message,
                        const Args &...Arguments) {
	Error("======= Panic =======");
	Error("{}{}", Message, Cxxutil::Forward<Args...>(Arguments...));
	Error("At file {} at line {} in function {}.", File, Line, Function);
	Error("CPU has been halted.");
	Error("Goodbye, world.");
	while(true) asm("cli\n hlt");
}

template<class... Args> [[noreturn]] void Panic(const char *Message, const Args &...Arguments) {
	Error("======= Panic =======");
	Error("{}{}", Message, Cxxutil::Forward<Args...>(Arguments...));
	Error("CPU has been halted.");
	Error("Goodbye, world.");
	while(true) asm("cli\n hlt");
}

extern "C" void AssertionFail(const char *Message,
                              const char *File,
                              const int Line,
                              const char *Function) {
	Panic(File, Line, Function, Message);
}
} // namespace Neon
