#include "Misc/Event.h"
#include "Misc/AssertionMacros.h"

namespace Bloodshot
{
	void FEventView::Subscribe(const uint32_t FuncUniqueID, std::function<void()>&& Func) const
	{
		BS_CHECK(Event);
		Event->Subscribe(FuncUniqueID, std::move(Func));
	}

	void FEventView::Unsubscribe(const uint32_t FuncUniqueID) const
	{
		BS_CHECK(Event);
		Event->Unsubscribe(FuncUniqueID);
	}
}
