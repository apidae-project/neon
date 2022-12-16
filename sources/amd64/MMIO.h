#pragma once

#include <Cxxshim/type_traits>
#include <Cxxshim/cstdint>

namespace Neon {
    template<typename type>
    concept valuetype = (std::is_same<type, uint8_t>() || std::is_same<type, uint16_t>() || std::is_same<type, uint32_t>() || std::is_same<type, uint64_t>());

    template<valuetype type>
    static inline type mmin(void *Addressess) {
        volatile auto ptr = reinterpret_cast<volatile type*>(Addressess);
        return *ptr;
    }

    template<valuetype type>
    static inline void mmout(void *Addressess, type value) {
        volatile auto ptr = reinterpret_cast<volatile type*>(Addressess);
        *ptr = value;
    }
}