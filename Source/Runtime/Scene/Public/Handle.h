#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FInstanceID final
	{
	public:
		operator uint64_t() const noexcept;

		bool operator==(const FInstanceID& Other);
		bool operator>=(const FInstanceID& Other);
		bool operator<=(const FInstanceID& Other);

		static constexpr uint64_t None = std::numeric_limits<uint64_t>::max();
		uint64_t Value = None;

		bool IsValid() const noexcept;
		void Reset() noexcept;
	};

	class FTypeID final
	{
	public:
		operator uint64_t() const noexcept;

		bool operator==(const FTypeID& Other);
		bool operator>=(const FTypeID& Other);
		bool operator<=(const FTypeID& Other);

		static constexpr uint64_t None = std::numeric_limits<uint64_t>::max();
		uint64_t Value = None;

		template<typename T>
		NODISCARD static FTypeID Get()
		{
			static const FTypeID TypeID{TypeIDCounter++};
			return TypeID;
		}

		template<typename FamilyType, typename T>
		NODISCARD static FTypeID Get()
		{
			static const FTypeID TypeID{TypeIDCounter++};
			return TypeID;
		}

		bool IsValid() const noexcept;
		void Reset() noexcept;

	private:
		static inline uint64_t TypeIDCounter = 0;
	};
}

namespace std
{
	template<>
	struct hash<Bloodshot::FInstanceID>
	{
		uint64_t operator()(const Bloodshot::FInstanceID& InstanceID) const
		{
			return hash<uint64_t>{}(InstanceID.Value);
		}
	};

	template<>
	struct hash<Bloodshot::FTypeID>
	{
		uint64_t operator()(const Bloodshot::FTypeID& TypeID) const
		{
			return hash<uint64_t>{}(TypeID.Value);
		}
	};
}
