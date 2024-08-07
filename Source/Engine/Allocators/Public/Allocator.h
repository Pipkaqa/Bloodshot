#pragma once

namespace Bloodshot
{
	class IAllocator abstract
	{
	public:
		virtual ~IAllocator() {}

		virtual void* Allocate() = 0;
		virtual void Deallocate(void* const Block) = 0;
		virtual void Dispose() = 0;
	};
}
