#pragma once

#define BS_DEFINE_ENUM_CLASS_FLAGS(EnumType)                                \
    inline constexpr bool operator!(EnumType Enum)                          \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return !(UnderlyingType)Enum;                                       \
    }                                                                       \
    inline constexpr EnumType operator~(EnumType Enum)                      \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return (EnumType)~(UnderlyingType)Enum;                             \
    }                                                                       \
    inline constexpr EnumType operator&(EnumType Lhs, EnumType Rhs)         \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return (EnumType)((UnderlyingType)Lhs & (UnderlyingType)Rhs);       \
    }                                                                       \
    inline constexpr EnumType operator|(EnumType Lhs, EnumType Rhs)         \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return (EnumType)((UnderlyingType)Lhs | (UnderlyingType)Rhs);       \
    }                                                                       \
    inline constexpr EnumType operator^(EnumType Lhs, EnumType Rhs)         \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return (EnumType)((UnderlyingType)Lhs ^ (UnderlyingType)Rhs);       \
    }                                                                       \
    inline EnumType& operator&=(EnumType& Lhs, EnumType Rhs)                \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return Lhs = (EnumType)((UnderlyingType)Lhs & (UnderlyingType)Rhs); \
    }                                                                       \
    inline EnumType& operator|=(EnumType& Lhs, EnumType Rhs)                \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);				    \
        return Lhs = (EnumType)((UnderlyingType)Lhs | (UnderlyingType)Rhs); \
    }                                                                       \
    inline EnumType& operator^=(EnumType& Lhs, EnumType Rhs)                \
    {                                                                       \
        using UnderlyingType = __underlying_type(EnumType);                 \
        return Lhs = (EnumType)((UnderlyingType)Lhs ^ (UnderlyingType)Rhs); \
    }

namespace Bloodshot
{
	template<typename EnumType>
	constexpr bool EnumHasAnyFlags(EnumType Flags, const EnumType Contains)
	{
		using UnderlyingType = __underlying_type(EnumType);
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) != 0;
	}

	template<typename EnumType>
	constexpr bool EnumHasAllFlags(EnumType Flags, const EnumType Contains)
	{
		using UnderlyingType = __underlying_type(EnumType);
		return ((UnderlyingType)Flags & (UnderlyingType)Contains) == (UnderlyingType)Contains;
	}

	template<typename EnumType>
	void EnumAddFlags(EnumType& Flags, const EnumType FlagsToAdd)
	{
		using UnderlyingType = __underlying_type(EnumType);
		Flags = (EnumType)((UnderlyingType)Flags | (UnderlyingType)FlagsToAdd);
	}

	template<typename EnumType>
	void EnumRemoveFlags(EnumType& Flags, const EnumType FlagsToRemove)
	{
		using UnderlyingType = __underlying_type(EnumType);
		Flags = (EnumType)((UnderlyingType)Flags & ~(UnderlyingType)FlagsToRemove);
	}
}
