#pragma once

namespace Neon {
namespace Serial {
enum COMs {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8,
};

static void PrintCharacter(char Character, COMs COM = COM1);
static char ReadCharacter(COMs COM = COM1);
void InitializePort(COMs Port);
void Initialize();
} // namespace Serial
} // namespace Neon