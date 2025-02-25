#pragma once

#include "Containers/List.h"
#include "Platform/Platform.h"

#include <functional>

namespace Bloodshot
{
	class FEvent;

	class FEventView
	{
		friend class FEvent;

	public:
		FORCEINLINE FEventView() = default;

		FORCEINLINE FEventView(const FEventView& Other) 
			: Event(Other.Event)
		{
		}

		FORCEINLINE FEventView(FEventView&& Other) noexcept
			: Event(std::exchange(Other.Event, nullptr))
		{
		}

		FORCEINLINE FEventView& operator=(const FEventView& Other)
		{
			Event = Other.Event;
			return *this;
		}

		FORCEINLINE FEventView& operator=(FEventView&& Other) noexcept
		{
			Event = std::exchange(Other.Event, nullptr);
			return *this;
		}

		void Subscribe(const uint32_t FuncUniqueID, std::function<void()>&& Func) const;
		void Unsubscribe(const uint32_t FuncUniqueID) const;

	private:
		FORCEINLINE FEventView(FEvent* const InEvent)
			: Event(InEvent)
		{
		}

		FEvent* Event = nullptr;
	};

	class FEvent
	{
	public:
		NODISCARD FORCEINLINE FEventView GetView() noexcept
		{
			return FEventView(this);
		}

		FORCEINLINE void Subscribe(const uint32_t FuncUniqueID, std::function<void()>&& Func)
		{
			Subscriptions.emplace_back(FSubscription(FuncUniqueID, std::move(Func)));
		}

		FORCEINLINE void Unsubscribe(const uint32_t FuncUniqueID)
		{
			Subscriptions.remove_if([FuncUniqueID](const FSubscription& Sub)
			{
				return Sub.UniqueID == FuncUniqueID;
			});
		}

		FORCEINLINE void Trigger()
		{
			for (const FSubscription& Sub : Subscriptions)
			{
				Sub.Func();
			}
		}

	private:
		struct FSubscription
		{
			uint32_t UniqueID;
			std::function<void()> Func;
		};

		TList<FSubscription> Subscriptions;
	};
}
