#pragma once

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

        typedef struct [[gnu::packed]]{
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
    }
}