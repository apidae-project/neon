#pragma once

namespace MSR {
    static constexpr uint64_t CPUID_INVARIANT_TSC = (1 << 8);
    static constexpr uint64_t CPUID_TSC_DEADLINE = (1 << 24);
    static constexpr uint64_t CPUID_SMEP = (1 << 7);
    static constexpr uint64_t CPUID_SMAP = (1 << 20);
    static constexpr uint64_t CPUID_UMIP = (1 << 2);
    static constexpr uint64_t CPUID_X2APIC = (1 << 21);
    static constexpr uint64_t CPUID_GBPAGE = (1 << 26);

    struct [[gnu::packed]] registers_t {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
        uint64_t int_no, error_code, rip, cs, rflags, rsp, ss;
    };

    uint64_t rdmsr(uint32_t msr);
    void wrmsr(uint32_t msr, uint64_t value);
    void SetKernelGS(uint64_t address);
    uint64_t GetKernelGS();
    void SetGS(uint64_t address);
    uint64_t GetGS();
    void SetFS(uint64_t address);
    uint64_t GetFS();
    void wrxcr(uint32_t i, uint64_t value);
}