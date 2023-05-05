#include <Common/Bitmap.h>
#include <Common/Common.h>
#include <Common/Lock.h>
#include <Common/Log.h>
#include <Common/String.h>
#include <Core/Panic.h>
#include <algorithm>
#include <Core/PhysicalMemoryManager.h>

namespace Neon
{
namespace PhysicalMemoryManager
{
static uintptr_t UsableMemoryTop = 0;
uintptr_t MemoryTop = 0;
static lock_t *Lock;
static size_t TotalPages = 0;
static size_t UsablePages = 0;
static bitmap_t Bitmap;
static size_t LastIndex = 0;
static size_t UsedPages = 0;

size_t TotalMemory() { return TotalPages; }

size_t UsableMemory() { return UsablePages; }

size_t UsedMemory() { return UsedPages; }

size_t FreeMemory() { return UsablePages - UsedPages; }

void *Allocate(size_t Count)
{
	if (Count == 0) return nullptr;
	auto AllocateHelper = [Count](size_t Limit) -> void *
	{
		size_t p = 0;
		while (LastIndex < Limit)
		{
			if (!Bitmap[LastIndex++])
			{
				if (p++ == Count)
				{
					size_t Page = LastIndex - Count;
					for (size_t i = Page; i < LastIndex; i++) Bitmap[i] = true;
					return reinterpret_cast<void *>(Page * PageSize);
				}
			}
			else
				p = 0;
		}
		return nullptr;
	};
	void *Result = AllocateHelper(UsableMemoryTop / PageSize);
	memset(ToHigherHalf(Result), 0, Count * PageSize);
	size_t i = LastIndex;
	if (Result == nullptr)
	{
		LastIndex = 0;
		Result = AllocateHelper(i);
		if (Result == nullptr) Panic("Out of memory!");
	}
	UsedPages += Count * PageSize;
	return Result;
}

void Free(void *Pointer, size_t Count)
{
	if (Pointer == nullptr) return;
	Lock->Lock();
	size_t Page = reinterpret_cast<size_t>(Pointer) / PageSize;
	for (size_t i = Page; i < Page; i++) Bitmap[i] = false;
	UsedPages -= Count * PageSize;
}

void Initialize()
{
	Log("[PMM] Initializing...");
	limine_memmap_entry **MemoryMaps = MemoryMapRequest.response->entries;
	size_t MemoryMapCount = MemoryMapRequest.response->entry_count;

	for (size_t i = 0; i < MemoryMapCount; i++)
	{
		uintptr_t Top = MemoryMaps[i]->base + MemoryMaps[i]->length;
		MemoryTop = std::max(MemoryTop, Top);
		switch (MemoryMaps[i]->type)
		{
			case LIMINE_MEMMAP_USABLE:
				UsablePages += MemoryMaps[i]->length;
				UsableMemoryTop = std::max(UsableMemoryTop, Top);
				[[fallthrough]];
			case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
			case LIMINE_MEMMAP_KERNEL_AND_MODULES:
				TotalPages += MemoryMaps[i]->length;
				break;
		}
	}

	size_t BitmapSize = AlignUp((UsableMemoryTop / PageSize) / 8, PageSize);

	for (size_t i = 0; i < MemoryMapCount; i++)
	{
		if (MemoryMaps[i]->type != LIMINE_MEMMAP_USABLE) continue;

		if (MemoryMaps[i]->length >= BitmapSize)
		{
			Bitmap.Buffer =
			    reinterpret_cast<uint8_t *>(ToHigherHalf(MemoryMaps[i]->base));
			Bitmap.Size = BitmapSize;
			memset(Bitmap.Buffer, 0xFF, Bitmap.Size);
			MemoryMaps[i]->length -= BitmapSize;
			MemoryMaps[i]->base += BitmapSize;
			UsedPages += BitmapSize;
			break;
		}
	}

	for (size_t i = 0; i < MemoryMapCount; i++)
	{
		if (MemoryMaps[i]->type != LIMINE_MEMMAP_USABLE) continue;
		for (uintptr_t j = 0; j < MemoryMaps[i]->length; j += PageSize)
			Bitmap[(MemoryMaps[i]->base + j) / PageSize] = false;
	}

	MemoryTop = AlignUp(MemoryTop, 0x40000000);
	Log("[PMM] Done!");
}
} // namespace PhysicalMemoryManager
} // namespace Neon
