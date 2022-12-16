#pragma once

#include <Cxxshim/cstddef>
#include <Cxxshim/utility>
#include <Common/Function.h>
#include <amd64/MMIO.h>
#include <amd64/Cpuid.h>

namespace Neon {
    namespace Amd64 {
        static constexpr uint64_t CPUID_INVARIANT_TSC = (1 << 8);
        static constexpr uint64_t CPUID_TSC_DEADLINE = (1 << 24);
        static constexpr uint64_t CPUID_SMEP = (1 << 7);
        static constexpr uint64_t CPUID_SMAP = (1 << 20);
        static constexpr uint64_t CPUID_UMIP = (1 << 2);
        static constexpr uint64_t CPUID_X2APIC = (1 << 21);
        static constexpr uint64_t CPUID_GBPAGE = (1 << 26);

        enum PAT {
            Uncachable = 0x00,
            WriteCombining = 0x01,
            WriteThrough = 0x04,
            WriteProtected = 0x05,
            WriteBack = 0x06,
            Uncached = 0x07
        };

        typedef struct [[gnu::packed]] {
            uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
            uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
            uint64_t int_no, error_code, rip, cs, rflags, rsp, ss;
        } registers_t;

        uint64_t rdmsr(uint32_t msr);
        void wrmsr(uint32_t msr, uint64_t value);

        void SetKernelGS(uint64_t Address);
        uint64_t GetKernelGS();

        void SetGS(uint64_t Address);
        uint64_t GetGS();

        void SetFS(uint64_t Address);
        uint64_t GetFS();

        void WriteCR(uint64_t reg, uint64_t val);
        uint64_t ReadCR(uint64_t reg);

        void wrxcr(uint32_t i, uint64_t value);

        void xsaveopt(uint8_t *region);
        void xsave(uint8_t *region);
        void xrstor(uint8_t *region);
        void fxsave(uint8_t *region);
        void fxrstor(uint8_t *region);

        void invlpg(uint64_t Address);

        void EnableSSE();
        void EnableSMEP();
        void EnableSMAP();
        void EnableUMIP();
        void EnablePAT();
        void outb(uint16_t Port, uint8_t Value);
        void outw(uint16_t Port, uint16_t Value);
        void outl(uint16_t Port, uint32_t Value);
        uint8_t inb(uint16_t Port);
        uint16_t inw(uint16_t Port);
        uint32_t inl(uint16_t Port);
        void Wait(void);
        void EnableCPUFeatures();

        #define ReadGS(offset) \
        ({ \
            uint64_t value; \
            asm volatile("movq %%gs:[" #offset "], %0" : "=r"(value) :: "memory"); \
            value; \
        })

        #define ReadCR(num) \
        ({ \
            uint64_t value; \
            asm volatile("movq %%cr" #num ", %0" : "=r" (value) :: "memory"); \
            value; \
        })

        #define WriteCR(num, value) \
        { \
            asm volatile("movq %0, %%cr" #num :: "r" (value) : "memory"); \
        }

        namespace GDT {
            static constexpr uint8_t GDT_NULL = 0x00;
            static constexpr uint8_t GDT_CODE = 0x08;
            static constexpr uint8_t GDT_DATA = 0x10;
            static constexpr uint8_t GDT_USER_CODE = 0x18;
            static constexpr uint8_t GDT_USER_DATA = 0x20;
            static constexpr uint8_t GDT_TSS = 0x28;

            struct [[gnu::packed]] GDTR {
                uint16_t Size;
                uint64_t Offset;
            };

            struct [[gnu::packed]] GDTEntry {
                uint16_t Limit0;
                uint16_t Base0;
                uint8_t Base1;
                uint8_t Access;
                uint8_t Granularity;
                uint8_t Base2;
            };

            struct [[gnu::packed]] TSSEntry {
                uint16_t Length;
                uint16_t Base0;
                uint8_t Base1;
                uint8_t Flags1;
                uint8_t Flags2;
                uint8_t Base2;
                uint32_t Base3;
                uint32_t Reserved;
            };

            struct [[gnu::packed]] GDT {
                GDTEntry Null;
                GDTEntry Code;
                GDTEntry Data;
                GDTEntry UserCode;
                GDTEntry UserData;
                TSSEntry Tss;
            };

            struct [[gnu::packed]] TSS {
                uint32_t Reserved0;
                uint64_t RSP[3];
                uint64_t Reserved1;
                uint64_t IST[7];
                uint64_t Reserved2;
                uint16_t Reserved3;
                uint16_t IOPBOffset;
            };

            extern TSS *tss;

            void Initialize(void);
        }

        namespace IDT {
            using namespace GDT;
            struct InterruptFrame {
                uint64_t error_code;
                uint64_t rip;
                uint64_t cs;
                uint64_t rflags;
                uint64_t rsp;
                uint64_t ss;
            };

            struct [[gnu::packed]] IDTDescriptor {
                uint16_t limit;
                uint64_t offset;
            };

            struct [[gnu::packed]] IDTGate {
                uint16_t offset1;
                uint16_t selector;
                uint8_t ist;
                uint8_t type;
                uint16_t offset2;
                uint32_t offset3;
                uint32_t zero;
            };

            struct [[gnu::packed]] IDTEntry {
                uint16_t Offset1;
                uint16_t Selector;
                uint8_t IST;
                uint8_t TypeAttr;
                uint16_t Offset2;
                uint32_t Offset3;
                uint32_t Zero;     
                void Set(void *isr, uint8_t typeattr = 0x8E, uint8_t ist = 0) {
                    uint64_t israddr = reinterpret_cast<uint64_t>(isr);
                    this->Offset1 = static_cast<uint16_t>(israddr);
                    this->Selector = GDT_CODE;
                    this->IST = ist;
                    this->TypeAttr = typeattr;
                    this->Offset2 = static_cast<uint16_t>(israddr >> 16);
                    this->Offset3 = static_cast<uint32_t>(israddr >> 32);
                    this->Zero = 0;
                }
            };

            struct [[gnu::packed]] IDTPointer {
                uint16_t Limit;
                uint64_t Base;
            };

            class InterruptHandler {
                    std::Function<void(registers_t*)> Handler;
                public:
                    template<typename Function, typename ...Args>
                    bool Set(Function &&func, Args ...args) {
                        if (this->Handler == true) return false;
                        this->Handler = [func = std::forward<Function>(func), args...](registers_t *Registers) mutable {
                            func(Registers, std::move(args)...);
                        };
                        return true;
                    }

                    bool Clear();
                    bool Get();
                    bool operator()(registers_t *regs);
            };

            void Initialize(void);
        }
    }
}