#pragma once

#include <Common/Log.h>
#include <cstdint>

namespace Neon {
namespace Devices {
namespace NVMe {
constexpr int CLASS = 0x01;
constexpr int SUBCLASS = 0x08;
constexpr int PROGIF = 0x02;
constexpr int QUEUE_PHYS_CONTIG = (1 << 0);
constexpr int SQ_PRIO_MEDIUM = (2 << 1);
constexpr int CQ_IRQ_ENABLED = (1 << 1);
constexpr int FEAT_NUM_QUEUES = 0x07;

#define NVME_CAP_MQES(Cap) ((Cap)&0xffff)
#define NVME_CAP_TIMEOUT(Cap) (((Cap) >> 24) & 0xff)
#define NVME_CAP_STRIDE(Cap) (((Cap) >> 32) & 0xf)
#define NVME_CAP_MPSMIN(Cap) (((Cap) >> 48) & 0xf)
#define NVME_CAP_MPSMAX(Cap) (((Cap) >> 52) & 0xf)

enum {
	PS_FLAGS_MAX_POWER_SCALE = 1 << 0,
	PS_FLAGS_NON_OP_STATE = 1 << 1,
};

struct PowerState {
	uint16_t MaxPower;
	uint8_t Reserved2;
	uint8_t Flags;
	uint32_t EntryLatency;
	uint32_t ExitLatency;
	uint8_t ReadThroughput;
	uint8_t ReadLatency;
	uint8_t WriteThroughput;
	uint8_t WriteLatency;
	uint16_t ActivePower;
	uint8_t IdleScale;
	uint8_t rsdv19;
	uint16_t ActivePower;
	uint8_t ActiveWorkScale;
	uint8_t rsvd23[9];
};

struct ControlID {
	uint16_t vid;
	uint16_t ssvid;
	char sn[20];
	char mn[40];
	char fn[8];
	uint8_t rab;
	uint8_t ieee[3];
	uint8_t mic;
	uint8_t mdts;
	uint16_t cntlid;
	uint32_t ver;
	uint8_t Reserved84[179];
	uint16_t oacs;
	uint8_t acl;
	uint8_t aerl;
	uint8_t frmw;
	uint8_t lpa;
	uint8_t elpe;
	uint8_t npss;
	uint8_t avscc;
	uint8_t apsta;
	uint16_t wctemp;
	uint16_t cctemp;
	uint8_t Reserved270[242];
	uint8_t sqes;
	uint8_t cqes;
	uint8_t Reserved514[2];
	uint32_t nn;
	uint16_t oncs;
	uint16_t fuses;
	uint8_t fna;
	uint8_t vwc;
	uint16_t awun;
	uint16_t awupf;
	uint8_t nvscc;
	uint8_t rsvd531;
	uint16_t acwu;
	uint8_t Reserved534[2];
	uint32_t sgls;
	uint8_t Reserved540[1508];
	PowerState psd[32];
	uint8_t vs[1024];
};

struct Completion {
	uint32_t Result;
	uint32_t Reserved;
	uint16_t SQHead;
	uint16_t SQID;
	uint16_t CommandID;
	uint16_t Status;
};

enum {
	NS_FEAT_THIN = 1 << 0,
	NS_FLBAS_LBA_MASK = 0xf,
	NS_FLBAS_META_EXT = 0x10,
	LBAF_RP_BEST = 0,
	LBAF_RP_BETTER = 1,
	LBAF_RP_GOOD = 2,
	LBAF_RP_DEGRADED = 3,
	NS_DPC_PI_LAST = 1 << 4,
	NS_DPC_PI_FIRST = 1 << 3,
	NS_DPC_PI_TYPE3 = 1 << 2,
	NS_DPC_PI_TYPE2 = 1 << 1,
	NS_DPC_PI_TYPE1 = 1 << 0,
	NS_DPS_PI_FIRST = 1 << 3,
	NS_DPS_PI_MASK = 0x7,
	NS_DPS_PI_TYPE1 = 1,
	NS_DPS_PI_TYPE2 = 2,
	NS_DPS_PI_TYPE3 = 3,
};

struct LBAFormat {
	uint16_t ms;
	uint8_t ds;
	uint8_t rp;
};

enum class Opcode {
	Flush = 0x00,
	Write = 0x01,
	Read = 0x02,
	Write_uncor = 0x04,
	Compare = 0x05,
	WriteZeroes = 0x08,
	DSM = 0x09,
	RESVCRegister = 0x0d,
	RESVReport = 0x0e,
	RESVAcquire = 0x11,
	RESVRelease = 0x15,
};

enum {
	CC_ENABLE = 1 << 0,
	CC_CSS_NVM = 0 << 4,
	CC_MPS_SHIFT = 7,
	CC_ARB_RR = 0 << 11,
	CC_ARB_WRRU = 1 << 11,
	CC_ARB_VS = 7 << 11,
	CC_SHN_NONE = 0 << 14,
	CC_SHN_NORMAL = 1 << 14,
	CC_SHN_ABRUPT = 2 << 14,
	CC_SHN_MASK = 3 << 14,
	CC_IOSQES = 6 << 16,
	CC_IOCQES = 4 << 20,
	CSTS_RDY = 1 << 0,
	CSTS_CFS = 1 << 1,
	CSTS_SHST_NORMAL = 0 << 2,
	CSTS_SHST_OCCUR = 1 << 2,
	CSTS_SHST_CMPLT = 2 << 2,
	CSTS_SHST_MASK = 3 << 2,
};

class NVMeDevice {
	Bar *Bar_;
	uint32_t BlockSize;
	uint32_t MaxTransferSize;
	struct Bar {
		uint64_t Capabilities;  /* Controller Capabilities */
		uint32_t Version;       /* Version */
		uint32_t InterruptMaskSet;         /* Interrupt Mask Set */
		uint32_t InterruptMaskClear;         /* Interrupt Mask Clear */
		uint32_t Configuration; /* Controller Configuration */
		uint32_t Reserved1;      /* Reserved */
		uint32_t Status;        /* Controller Status */
		uint32_t Reserved2;         /* Reserved */
		uint32_t AQAttributes;  /* Admin Queue Attributes */
		uint64_t AdminSQ;       /* Admin SQ Base Address */
		uint64_t AdminCQ;       /* Admin CQ Base Address */
	};

	struct [[gnu::packed]] Command {
		union [[gnu::packed]] {
			struct [[gnu::packed]] {
				uint32_t Opcode : 8;
				uint32_t FusedOperation : 2;
				uint32_t Reserved0 : 5;
				uint32_t PRP : 1;
				uint32_t CommandDword10 : 16;
			} NVM;
			struct [[gnu::packed]] {
				uint32_t Opcode : 8;
				uint32_t FusedOperation : 2;
				uint32_t Reserved0 : 5;
				uint32_t PRP : 1;
				uint32_t CommandDword10 : 16;
			} Admin;
		} CommandDword0;
		uint32_t CommandDword1;
		uint64_t MetadataPointer;
		uint64_t PRPEntry[2];
		uint32_t CommandDword10;
		uint32_t CommandDword11;
		uint32_t CommandDword12;
		uint32_t CommandDword13;
		uint32_t CommandDword14;
		uint32_t CommandDword15;
	};

	struct NamespaceInfo {
		uint64_t NamespaceSize;
		uint64_t NamespaceCapacity;
		uint64_t NamespaceUtilization;
		uint32_t NamespaceID;
		uint32_t NamespaceFormat;
		uint8_t NamespaceName[20];
	};
public:
	NVMeDevice(Bar *Bar_) : Bar_(Bar_) {
		this->Bar_->Configuration = 0;
		this->Bar_->InterruptMaskSet = 0;
		this->Bar_->InterruptMaskClear = 0;
		this->Bar_->AQAttributes = 0;
		NamespaceInfo Info;
		if(!GetNamespaceInfo(0, &Info)) Error("[NVMe] Could not get namespace info.");
		BlockSize = Info.NamespaceSize;
		MaxTransferSize = BlockSize * Info.NamespaceCapacity;
	}
	NVMeDevice() = default;
	bool GetNamespaceInfo(uint32_t NamespaceID, NVMeNamespaceInfo *Info);
	uint64_t GetNamespaceSize(uint32_t NamespaceID);
	bool Read(uint64_t LBA, uint32_t Sectors, void *Data);
	bool Write(uint64_t LBA, uint32_t Sectors, const void *Data);
};

void Initialize();
} // namespace NVMe
} // namespace Devices
} // namespace Neon
