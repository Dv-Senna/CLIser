#pragma once

#include <cinttypes>



#define CLI_CREATE_BITFIELD(name, ...) enum class name##Bits {__VA_ARGS__};\
	struct name {\
		std::uint64_t value;\
		name() noexcept = default;\
		inline name(name##Bits value) noexcept : value {(std::uint64_t)value} {}\
		inline name(const name &copy) noexcept : value {copy.value} {}\
		inline const name &operator=(const name &copy) noexcept {value = copy.value; return *this;}\
\
		inline operator name##Bits() const noexcept {return (name##Bits)value;}\
\
		inline const name &operator|=(const name &other) noexcept {value |= other.value; return *this;}\
		inline const name &operator|=(name##Bits flag) noexcept {value |= (std::uint64_t)flag; return *this;}\
		inline const name &operator&=(const name &other) noexcept {value &= other.value; return *this;}\
		inline const name &operator&=(name##Bits flag) noexcept {value &= (std::uint64_t)flag; return *this;}\
		inline const name &operator^=(const name &other) noexcept {value ^= other.value; return *this;}\
		inline const name &operator^=(name##Bits flag) noexcept {value ^= (std::uint64_t)flag; return *this;}\
\
		inline name operator|(const name &other) noexcept {auto copy {*this}; return copy |= other;}\
		inline name operator|(name##Bits flag) noexcept {auto copy {*this}; return copy |= flag;}\
		inline name operator&(const name &other) noexcept {auto copy {*this}; return copy &= other;}\
		inline name operator&(name##Bits flag) noexcept {auto copy {*this}; return copy &= flag;}\
		inline name operator^(const name &other) noexcept {auto copy {*this}; return copy ^= other;}\
		inline name operator^(name##Bits flag) noexcept {auto copy {*this}; return copy ^= flag;}\
\
		inline name operator~() noexcept {return name((name##Bits)~value);}\
\
		inline bool operator==(const name &other) noexcept {return value == other.value;}\
		inline bool operator==(name##Bits flag) noexcept {return value == (std::uint64_t)flag;}\
		inline operator bool() const noexcept {return value != 0;}\
	};\
\
	inline name operator~(name##Bits flag) {return ~name(flag);}


