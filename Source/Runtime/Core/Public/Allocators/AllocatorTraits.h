#pragma once

namespace Bloodshot
{
	template<IsAllocator InAllocatorType>
	class IAllocatorTraits
	{
	public:
		using AllocatorType = InAllocatorType;
		using ElementType = AllocatorType::ElementType;
		using PointerType = AllocatorType::PointerType;
	};
}
