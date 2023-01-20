#pragma once

#include <LimineRequests.h>
#include <type_traits>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <cctype>
#include <limits>

namespace Neon {
    struct Point {
        size_t X = 0;
        size_t Y = 0;
    };

    constexpr auto AlignDown(auto n, auto a) {
        return (n & ~(a - 1));
    }

    constexpr auto AlignUp(auto n, auto a) {
        return AlignDown(n + a - 1, a);
    }

    constexpr auto DivRoundup(auto n, auto a) {
        return AlignDown(n + a - 1, a) / a;
    }

    template<typename Enum>
    constexpr auto AsInteger(const Enum Value) -> typename std::underlying_type<Enum>::type {
        return static_cast<typename std::underlying_type<Enum>::type>(Value);
    }

    template<std::integral Type>
    constexpr bool IsHigherHalf(Type a) {
        return static_cast<uintptr_t>(a) >= HHDMRequest.response->offset;
    }

    template<std::integral Type>
    constexpr Type ToHigherHalf(Type a) {
        return IsHigherHalf(a) ? a : static_cast<Type>(static_cast<uintptr_t>(a) + HHDMRequest.response->offset);
    }

    template<std::integral Type>
    constexpr Type FromHigherHalf(Type a) {
        return !IsHigherHalf(a) ? a : static_cast<Type>(static_cast<uintptr_t>(a) - HHDMRequest.response->offset);
    }

    template<typename Type>
    constexpr bool IsHigherHalf(Type a) {
        return reinterpret_cast<uintptr_t>(a) >= HHDMRequest.response->offset;
    }

    template<typename Type>
    constexpr Type ToHigherHalf(Type a) {
        return IsHigherHalf(a) ? a : reinterpret_cast<Type>(reinterpret_cast<uintptr_t>(a) + HHDMRequest.response->offset);
    }

    template<typename Type>
    constexpr Type FromHigherHalf(Type a) {
        return !IsHigherHalf(a) ? a : reinterpret_cast<Type>(reinterpret_cast<uintptr_t>(a) - HHDMRequest.response->offset);
    }

    template<typename Type>
    constexpr Type pow(Type Base, Type Exponent) {
        int Result = 1;
        while(Exponent > 0) {
            Result *= Base;
            Exponent--;
        }
        return Result;
    }

    template<typename Type>
    constexpr Type abs(Type Number) {
        return Number < 0 ? -Number : Number;
    }

    template<typename Type>
    constexpr Type sign(Type Number) {
        return (Number > 0) ? 1 : ((Number < 0) ? -1 : 0);
    }

    constexpr uint64_t jdn(uint8_t days, uint8_t months, uint16_t years) {
        return (1461 * (years + 4800 + (months - 14) / 12)) / 4 + (367 * (months - 2 - 12 * ((months - 14) / 12))) / 12 - (3 * ((years + 4900 + (months - 14) / 12) / 100)) / 4 + days - 32075;
    }

    constexpr uint64_t jdn_1970 = jdn(1, 1, 1970);

    constexpr uint64_t epoch(uint64_t seconds, uint64_t minutes, uint64_t hours, uint64_t days, uint64_t months, uint64_t years, uint64_t centuries) {
        uint64_t jdn_current = jdn(days, months, centuries * 100 + years);
        uint64_t diff = jdn_current - jdn_1970;

        return (diff * (60 * 60 * 24)) + hours * 3600 + minutes * 60 + seconds;
    }

    inline uint64_t SecondsSinceBoot(uint64_t seconds, uint64_t minutes, uint64_t hours, uint64_t days, uint64_t months, uint64_t years, uint64_t centuries) {
        return epoch(seconds, minutes, hours, days, months, years, centuries) - BootTimeRequest.response->boot_time;
    }
}