#include <Devices/Terminal.h>
#include <LimineRequests.h>
#include <Common/Vector.h>
#include <Common/Memory.h>
#include <limine-terminal-port/source/cpp/term.hpp>
#include <LimineRequests.h>
#include <limine.h>
#include <Core/Panic.h>
#include <Common/Log.h>
#include <Devices/Framebuffer.h>
#include <CString.h>

namespace Neon
{
namespace Devices
{
namespace Terminal
{
Vector<Terminal *> Terminals;
size_t TerminalCount = 0;
Terminal *MainTerminal = nullptr;
limine_terminal *EarlyTerminal = nullptr;

extern "C"
{
	void *alloc_mem(size_t count) { return Heap.AllocateZeroed(count); }

	void free_mem(void *ptr, size_t count)
	{
		(void)count;
		Heap.Free(ptr);
	}
}

void WriteLine(const char *Line, Terminal *Terminal_)
{
	if (Terminal_ == nullptr && EarlyTerminal != nullptr)
		TerminalRequest.response->write(EarlyTerminal, Line, strlen(Line));
	Terminal_->write(Line, strlen(Line));
}

void WriteCharacter(char Character, Terminal *Terminal_)
{
	if (Terminal_ == nullptr && EarlyTerminal != nullptr)
		TerminalRequest.response->write(EarlyTerminal, &Character, 1);
	Terminal_->write(&Character, 1);
}

void Terminal::Callback(Terminal *Terminal_,
                        uint64_t Type,
                        uint64_t First,
                        uint64_t Second,
                        uint64_t Third)
{
	auto _Terminal = reinterpret_cast<Terminal *>(Terminal_);
	switch (Type)
	{
		case TERM_CB_DEC:
			break;
		case TERM_CB_BELL:
			break;
		case TERM_CB_PRIVATE_ID:
			break;
		case TERM_CB_STATUS_REPORT:
			break;
		case TERM_CB_POS_REPORT:
			_Terminal->Position.X = First;
			_Terminal->Position.Y = Second;
			break;
		case TERM_CB_KBD_LEDS:
			break;
		case TERM_CB_LINUX:
			break;
	}
}

void Initialize()
{
	Log("[Terminal] Initializing...");
	if (TerminalRequest.response != nullptr)
		EarlyTerminal = TerminalRequest.response->terminals[0];
	if (Framebuffer::FramebufferCount == 0)
	{
		Panic("[Terminal] No frambuffer(s) found!");
	}
	MainTerminal = Terminals[0];
	TerminalCount = TerminalRequest.response->terminal_count;
	Log("[Terminal] Done!");
}
} // namespace Terminal
} // namespace Devices
} // namespace Neon
