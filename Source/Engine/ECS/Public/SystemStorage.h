#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

#include <unordered_map>

namespace Bloodshot
{
	class IScene;
	class ISystem;

	struct FSystemStorage final
	{
	public:
		FSystemStorage(IScene* Context);
		~FSystemStorage();

		using FTypeIDSystemUnorderedMap = std::unordered_map<TypeID_t, ISystem*>;
		using FSystemsVec = std::vector<ISystem*>;

		IScene* Context;

		FTypeIDSystemUnorderedMap SystemsUnorderedMap;

		// BSTODO: Real work order
		FSystemsVec SystemsWorkOrder;

		void Store(ISystem* const System, const TypeID_t SystemTypeID);
		void Unstore(const TypeID_t SystemTypeID);
		void Clear();

		NODISCARD ISystem* Get(const TypeID_t SystemTypeID);
		NODISCARD const ISystem* Get(const TypeID_t SystemTypeID) const;

		NODISCARD FSystemsVec& GetSystemsVec();
		NODISCARD const FSystemsVec& GetSystemsVec() const;

		NODISCARD bool Contains(const TypeID_t SystemTypeID) const;
	};
}
