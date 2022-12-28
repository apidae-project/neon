#pragma once

#include <Cxx/type_traits>
#include <Cxx/cstdint>

namespace Neon {
    namespace Amd64 {
        template<typename type>
        concept valuetype = (std::is_same<type, uint8_t>() || std::is_same<type, uint16_t>() || std::is_same<type, uint32_t>() || std::is_same<type, uint64_t>());

        template<valuetype type>
        static inline type mmin(void *Address) {
            volatile auto ptr = reinterpret_cast<volatile type*>(Address);
            return *ptr;
        }

        template<valuetype type>
        static inline void mmout(void *Address, type value) {
            volatile auto ptr = reinterpret_cast<volatile type*>(Address);
            *ptr = value;
        }
    }
}