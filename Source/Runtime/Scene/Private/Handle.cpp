#include "Handle.h"

namespace Bloodshot
{
	FInstanceID::operator uint64_t() const noexcept
	{
		return Value;
	}
	
	bool FInstanceID::operator==(const FInstanceID& Other)
	{
		return Value == Other.Value;
	}

	bool FInstanceID::operator>=(const FInstanceID& Other)
	{
		return Value >= Other.Value;
	}

	bool FInstanceID::operator<=(const FInstanceID& Other)
	{
		return Value <= Other.Value;
	}

	bool FInstanceID::IsValid() const noexcept
	{
		return Value != None;
	}

	void FInstanceID::Reset() noexcept
	{
		Value = None;
	}

	FTypeID::operator uint64_t() const noexcept
	{
		return Value;
	}

	bool FTypeID::operator==(const FTypeID& Other)
	{
		return Value == Other.Value;
	}

	bool FTypeID::operator>=(const FTypeID& Other)
	{
		return Value >= Other.Value;
	}

	bool FTypeID::operator<=(const FTypeID& Other)
	{
		return Value <= Other.Value;
	}

	bool FTypeID::IsValid() const noexcept
	{
		return Value != None;
	}

	void FTypeID::Reset() noexcept
	{
		Value = None;
	}
}
