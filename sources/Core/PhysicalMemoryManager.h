#pragma once

namespace Neon
{
namespace PhysicalMemoryManager
{
static constexpr size_t PageSize = 0x1000;
template <typename Type = void *> Type Allocate(size_t Count = 1)
{
	return reinterpret_cast<Type>(Allocate(Count));
}
void *Allocate(size_t Count);
void Free(void *Pointer, size_t Count);
void Initialize();
} // namespace PhysicalMemoryManager
} // namespace Neon
