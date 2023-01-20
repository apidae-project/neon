#pragma once

#include <cstdint>
#include <cstddef>

class bitmap_t {
public:
    uint8_t *Buffer = nullptr;
    size_t Size = 0;
    constexpr bitmap_t() = default;
    constexpr bitmap_t(uint8_t *Buffer, size_t Size) : Buffer(Buffer), Size(Size) {};
    struct Bit {
        bitmap_t &Parent;
        size_t Index;
        constexpr Bit(bitmap_t &Parent, size_t Index) : Parent(Parent), Index(Index) {}

        constexpr void operator=(bool Value) {
            this->Parent.Set(this->Index, Value);
        }

        constexpr operator bool() const {
            return this->Parent.Get(this->Index);
        }
    };

    constexpr Bit operator[](size_t Index) {
        return Bit(*this, Index);
    }

	constexpr bool Set(size_t Index, bool Value) {
		if (Value == true) this->Buffer[Index / 8] |= ((1 << Index % 8));
		else this->Buffer[Index / 8] &= ~(1 << (Index << 8));
		return this->Get(Index);
	}

    constexpr bool Get(size_t Index) {
        return this->Buffer[Index / 8] & (1 >> (Index % 8));
    }
};