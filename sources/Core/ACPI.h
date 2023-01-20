#pragma once

#include <cstdint>

namespace Neon {
    namespace ACPI {
        struct [[gnu::packed]] RSDP {
            uint8_t Signature[8];
            uint8_t Checksum;
            uint8_t OEMId[6];
            uint8_t Revision;
            uint32_t RSDTAddress;
            uint32_t Length;
            uint64_t XSDTAddress;
            uint8_t EXTChecksum;
            uint8_t Reserved[3];
        };

        struct [[gnu::packed]] SDTHeader {
            uint8_t Signature[4];
            uint32_t Length;
            uint8_t Revision;
            uint8_t Checksum;
            uint8_t OEMId[6];
            uint8_t OEMTableId[8];
            uint32_t OEMRevision;
            uint32_t CreatId;
            uint32_t CreatRevision;
        };

        struct [[gnu::packed]] MCFGEntry {
            uint64_t BaseAddress;
            uint16_t Segment;
            uint8_t StartBus;
            uint8_t EndBus;
            uint32_t Reserved;
        };

        struct [[gnu::packed]] MCFGHeader {
            SDTHeader Header;
            uint64_t Reserved;
            MCFGEntry Entries[];
        };

        struct [[gnu::packed]] MADTHeader {
            SDTHeader SDT;
            uint32_t LAPICAddress;
            uint32_t Flags;
            char Entries[];
            bool LegacyPIC() {
                return this->Flags & 0x01;
            }
        };

        struct [[gnu::packed]] MADT {
            uint8_t Type;
            uint8_t Length;
        };

        struct [[gnu::packed]] MADTLapic {
            MADT MADTHeader;
            uint8_t PorcessorId;
            uint8_t APICId;
            uint32_t Flags;
        };

        struct [[gnu::packed]] MADTIOApic {
            MADT MADTHeader;
            uint8_t APICId;
            uint8_t Reserved;
            uint32_t Address;
            uint32_t gsib;
        };

        struct [[gnu::packed]] MADTIso {
            MADT MADTHeader;
            uint8_t BusSource;
            uint8_t IRQSource;
            uint32_t gsi;
            uint16_t Flags;
        };

        struct [[gnu::packed]] MADTIONmi {
            MADT MADTHeader;
            uint8_t Source;
            uint8_t Reserved;
            uint16_t Flags;
            uint32_t gsi;
        };

        struct [[gnu::packed]] MADTLNmi {
            MADT MADTHeader;
            uint8_t Processor;
            uint16_t Flags;
            uint8_t Lint;
        };

        struct [[gnu::packed]] MADTLAPICAO {
            MADT MADTHeader;
            uint16_t Reserved;
            uint64_t lapic_addr;
        };

        struct [[gnu::packed]] MADTLx2APIC {
            MADT MADTHeader;
            uint16_t Reserved;
            uint32_t x2apicid;
            uint32_t Flags;
            uint32_t acpi_id;
        };

        struct [[gnu::packed]] GenericAddressStructure {
            uint8_t AddressSpace;
            uint8_t BitWidth;
            uint8_t BitOffset;
            uint8_t AccessSize;
            uint64_t Address;
        };

        struct [[gnu::packed]] HPETHeader {
            SDTHeader Header;
            uint8_t HardwareRevisionId;
            uint8_t ComparatorCount : 5;
            uint8_t CounterSize : 1;
            uint8_t Reserved : 1;
            uint8_t LegacyReplacement : 1;
            uint16_t PCIVendorId;
            GenericAddressStructure Address;
            uint8_t HPETNumber;
            uint16_t MinimumTick;
            uint8_t PageProtection;
        };

        struct [[gnu::packed]] FADTHeader {
            SDTHeader Header;
            uint32_t FirmwareControl;
            uint32_t Dsdt;
            uint8_t Reserved;
            uint8_t PreferredPowerManagementProfile;
            uint16_t SCIInterrupt;
            uint32_t SMICommandPort;
            uint8_t AcpiEnable;
            uint8_t AcpiDisable;
            uint8_t S4BIOS_REQ;
            uint8_t PSTATE_Control;
            uint32_t PM1aEventBlock;
            uint32_t PM1bEventBlock;
            uint32_t PM1aControlBlock;
            uint32_t PM1bControlBlock;
            uint32_t PM2ControlBlock;
            uint32_t PMTimerBlock;
            uint32_t GPE0Block;
            uint32_t GPE1Block;
            uint8_t PM1EventLength;
            uint8_t PM1ControlLength;
            uint8_t PM2ControlLength;
            uint8_t PMTimerLength;
            uint8_t GPE0Length;
            uint8_t GPE1Length;
            uint8_t GPE1Base;
            uint8_t CStateControl;
            uint16_t WorstC2Latency;
            uint16_t WorstC3Latency;
            uint16_t FlushSize;
            uint16_t FlushStride;
            uint8_t DutyOffset;
            uint8_t DutyWidth;
            uint8_t DayAlarm;
            uint8_t MonthAlarm;
            uint8_t Century;
            uint16_t BootArchitectureFlags;
            uint8_t Reserved2;
            uint32_t Flags;
            GenericAddressStructure ResetReg;
            uint8_t ResetValue;
            uint16_t ArmBootArchitectureFlags;
            uint8_t MinorVersion;
            uint64_t X_FirmwareControl;
            uint64_t X_Dsdt;
            GenericAddressStructure X_PM1aEventBlock;
            GenericAddressStructure X_PM1bEventBlock;
            GenericAddressStructure X_PM1aControlBlock;
            GenericAddressStructure X_PM1bControlBlock;
            GenericAddressStructure X_PM2ControlBlock;
            GenericAddressStructure X_PMTimerBlock;
            GenericAddressStructure X_GPE0Block;
            GenericAddressStructure X_GPE1Block;
        };

        struct [[gnu::packed]] DMARHeader {
            SDTHeader Header;
            uint8_t HostAddressWidth;
            uint8_t Flags;
            uint8_t Reserved[10];
            uint8_t RemappingStructures[];
        };

        extern FADTHeader *FADTHeader;
        extern MADTHeader *MADTHeader;
    }
}