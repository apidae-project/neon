#pragma once

#include <cstdint>

namespace Neon {
namespace ELF {
struct ElfHeader {
	uint8_t  Ident[16];
	uint16_t Type;
	uint16_t Machine;
	uint32_t Version;
	uint64_t Entry;
	uint64_t PHOffset;
	uint64_t SHOffset;
	uint32_t Flags;
	uint16_t Size;
	uint16_t PHEntrySize;
	uint16_t PHNumber;
	uint16_t SHEntrySize;
	uint16_t SHNumber;
	uint16_t e_shstrndx;
};

struct ProgramHeader {
	uint32_t Type;
	uint32_t Flags;
	uint64_t Offset;
	uint64_t VirtualAddress;
	uint64_t PhysicalAddress;
	uint64_t FileSize;
	uint64_t MemorySize;
	uint64_t Alignment;
};

struct SectionHeader {
    uint32_t Name;
    uint32_t Type;
    uint64_t Flags;
    uint64_t Address;
    uint64_t Offset;
    uint64_t Size;
    uint32_t Link;
    uint32_t Info;
    uint64_t AddressAlignment;
    uint64_t EntrySize;
};
} // namespace ELF
} // namespace Neon
