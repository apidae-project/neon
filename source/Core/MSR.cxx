#include <Common/cpuid.h>
#include <Core/MSR.hxx>

namespace MSR {
    uint64_t rdmsr(uint32_t msr) {
        int32_t edx, eax;
        asm volatile("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr) : "memory");
        return (static_cast<uint64_t>(edx) << 32) | eax;
    }

    void wrmsr(uint32_t msr, uint64_t value) {
        uint32_t edx = value >> 32;
        uint32_t eax = static_cast<uint32_t>(value);
        asm volatile("wrmsr" :: "a"(eax), "d"(edx), "c"(msr) : "memory");
    }

    void SetKernelGS(uint64_t address) {
        wrmsr(0xC0000102, address);
    }

    uint64_t GetKernelGS() {
        return rdmsr(0xC0000102);
    }   

    void SetGS(uint64_t address) {
        wrmsr(0xC0000101, address);
    }

    uint64_t GetGS() {
        return rdmsr(0xC0000101);
    }

    void SetFS(uint64_t address) {
        wrmsr(0xC0000100, address);
    }   

    uint64_t GetFS() {
        return rdmsr(0xC0000100);
    }

    void wrxcr(uint32_t i, uint64_t value) {
        uint32_t edx = value >> 32;
        uint32_t eax = static_cast<uint32_t>(value);
        asm volatile ("xsetbv" :: "a"(eax), "d"(edx), "c"(i) : "memory");
    }
}