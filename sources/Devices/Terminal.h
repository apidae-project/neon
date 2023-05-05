#pragma once

#include <Common/Common.h>
#include <Common/Vector.h>
#include <Common/Lock.h>
#include <limine-terminal-port/source/cpp/term.hpp>
#include <cstdint>
#include <limine.h>

namespace Neon
{
namespace Devices
{
namespace Terminal
{
struct Terminal : cppterm_t
{
	Point Position{0, 0};
	lock_t Lock;
	Point GetPosition()
	{
		this->write("\033[6n", 5);
		return this->Position;
	}
	static void Callback(Terminal *Terminal_,
	                     uint64_t Type,
	                     uint64_t First,
	                     uint64_t Second,
	                     uint64_t Third);
};
extern Vector<Terminal *> Terminals;
extern size_t TerminalCount;
extern Terminal *MainTerminal;
extern limine_terminal *EarlyTerminal;
void WriteLine(const char *Line, Terminal Terminal_);
void WriteCharacter(char Character, Terminal Terminal_);
void Initialize();
} // namespace Terminal
} // namespace Devices
} // namespace Neon
