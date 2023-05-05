#include <Devices/NVMe.h>
#include <Common/CString.h>
#include <Vector.h>
#include <Common/Memory.h>
#include <Common/Log.h>
#include <cstddef>

namespace Neon {
namespace Devices {
namespace NVMe {
Cxxutil::Vector<NVMeDevice> Devices;

bool NVMeDevice::Read(uint64_t LBA, uint32_t Sectors, void *Data) {
	uint32_t TransferSize = Sectors * this->BlockSize;
	if(TransferSize > this->MaxTransferSize) return false;
	uint32_t NamespaceID = LBA >> this->NamespaceIDShift;
	if(NamespaceID >= this->NamespaceCount) return false;
	uint64_t Offset = LBA & ((1 << this->NamespaceIDShift) - 1);
	uint32_t DataSize = TransferSize + sizeof(Command);
	Command *Command = (Command *)Heap.Allocate(DataSize);
	memset(Command, 0, DataSize);
	Command->Opcode = NVME_READ;
	Command->NamespaceID = NamespaceID;
	Command->StartingLBA = Offset;
	Command->DataTransferLength = TransferSize;
	bool Success = this->SendCommand(Command, Data, DataSize);
	Heap.Free(Command);
	return Success;
}

bool NVMeDevice::Write(uint64_t LBA, uint32_t Sectors, const void *Data) {
	uint32_t TransferSize = Sectors * this->BlockSize;
	if(TransferSize > this->MaxTransferSize) return false;
	uint32_t NamespaceID = LBA >> this->NamespaceIDShift;
	if(NamespaceID >= this->NamespaceCount) return false;
	uint64_t Offset = LBA & ((1 << this->NamespaceIDShift) - 1);
	uint32_t DataSize = TransferSize + sizeof(Command);
	Command *Command = (Command *)Heap.Allocate(DataSize);
	memset(Command, 0, DataSize);
	Command->Opcode = NVME_WRITE;
	Command->NamespaceID = NamespaceID;
	Command->StartingLBA = Offset;
	Command->DataTransferLength = TransferSize;
	bool Success = this->SendCommand(Command, Data, DataSize);
	Heap.Free(Command);
	return Success;
}

bool NVMeDevice::GetNamespaceInfo(uint32_t NamespaceID, NamespaceInfo *Info) {
	uint32_t DataSize = sizeof(Command) + sizeof(NamespaceInfo);
	Command *Command = (Command *)Heap.Allocate(DataSize);
	memset(Command, 0, DataSize);
	Command->Opcode = NVME_IDENTIFY;
	Command->NamespaceID = NamespaceID;
	Command->CNS = NVME_IDENTIFY_NAMESPACE;
	bool Success = this->SendCommand(Command, Info, DataSize);
	Heap.Free(Command);
	return Success;
}

void Initialize() {
	Log("[NVMe] Discovering available devices...");
	uint32_t DeviceCount = 0;
}
} // namespace NVMe
} // namespace Devices
} // namespace Neon
