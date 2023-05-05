#pragma once

#include <LimineRequests.h>
#include <cctype>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace Neon {
struct Point {
	size_t X = 0;
	size_t Y = 0;
};

constexpr auto AlignDown(auto n, auto a) { return (n & ~(a - 1)); }

constexpr auto AlignUp(auto n, auto a) { return AlignDown(n + a - 1, a); }

constexpr auto DivRoundup(auto n, auto a) {
	return AlignDown(n + a - 1, a) / a;
}

template <typename Enum>
constexpr auto AsInteger(const Enum Value) ->
    typename std::underlying_type<Enum>::type {
	return static_cast<typename std::underlying_type<Enum>::type>(Value);
}

template <std::integral Type> constexpr bool IsHigherHalf(Type a) {
	return static_cast<uintptr_t>(a) >= HHDMRequest.response->offset;
}

template <std::integral Type> constexpr Type ToHigherHalf(Type a) {
	return IsHigherHalf(a) ? a
	                       : static_cast<Type>(static_cast<uintptr_t>(a) +
	                                           HHDMRequest.response->offset);
}

template <std::integral Type> constexpr Type FromHigherHalf(Type a) {
	return !IsHigherHalf(a) ? a
	                        : static_cast<Type>(static_cast<uintptr_t>(a) -
	                                            HHDMRequest.response->offset);
}

template <typename Type> constexpr bool IsHigherHalf(Type a) {
	return reinterpret_cast<uintptr_t>(a) >= HHDMRequest.response->offset;
}

template <typename Type> constexpr Type ToHigherHalf(Type a) {
	return IsHigherHalf(a)
	           ? a
	           : reinterpret_cast<Type>(reinterpret_cast<uintptr_t>(a) +
	                                    HHDMRequest.response->offset);
}

template <typename Type> constexpr Type FromHigherHalf(Type a) {
	return !IsHigherHalf(a)
	           ? a
	           : reinterpret_cast<Type>(reinterpret_cast<uintptr_t>(a) -
	                                    HHDMRequest.response->offset);
}

template <typename Type> constexpr Type pow(Type Base, Type Exponent) {
	int Result = 1;
	while (Exponent > 0)
	{
		Result *= Base;
		Exponent--;
	}
	return Result;
}

template <typename Type> constexpr Type abs(Type Number) {
	return Number < 0 ? -Number : Number;
}

template <typename Type> constexpr Type sign(Type Number) {
	return (Number > 0) ? 1 : ((Number < 0) ? -1 : 0);
}

constexpr uint64_t jdn(uint8_t Days, uint8_t Months, uint16_t Years) {
	return (1461 * (Years + 4800 + (Months - 14) / 12)) / 4 +
	       (367 * (Months - 2 - 12 * ((Months - 14) / 12))) / 12 -
	       (3 * ((Years + 4900 + (Months - 14) / 12) / 100)) / 4 + Days - 32075;
}

constexpr uint64_t jdn_1970 = jdn(1, 1, 1970);

constexpr uint64_t epoch(uint64_t Seconds, uint64_t Minutes, uint64_t Hours,
                         uint64_t Days, uint64_t Months, uint64_t Years,
                         uint64_t Centuries) {
	uint64_t jdn_current = jdn(Days, Months, Centuries * 100 + Years);
	uint64_t diff = jdn_current - jdn_1970;
	return (diff * (360 * 24)) + Hours * 3600 + Minutes * 60 + Seconds;
}
