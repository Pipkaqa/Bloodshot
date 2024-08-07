#include "SystemStorage.h"

#include "ISystem.h"
#include "Logging/LoggingMacros.h"
#include "Scene.h"

namespace Bloodshot
{
	FSystemStorage::FSystemStorage(IScene* Context)
		: Context(Context)
	{
		BS_LOG(Debug, "Creating SystemStorage on Scene of type: {0}...", Context->GetTypeName());
	}

	FSystemStorage::~FSystemStorage()
	{
		BS_LOG(Debug, "Destroying SystemStorage on Scene of type: {0}...", Context->GetTypeName());
	}

	void FSystemStorage::Store(ISystem* const System, const TypeID_t SystemTypeID)
	{
		System->UniqueID = SystemTypeID;

		SystemsUnorderedMap[SystemTypeID] = System;

		SystemsWorkOrder.push_back(System);
	}

	void FSystemStorage::Unstore(const TypeID_t SystemTypeID)
	{
		SystemsUnorderedMap.at(SystemTypeID) = nullptr;

		for (FSystemsVec::const_iterator It = SystemsWorkOrder.begin(); It != SystemsWorkOrder.end(); ++It)
		{
			if ((*It)->UniqueID == SystemTypeID)
			{
				SystemsWorkOrder.erase(It);
				return;
			}
		}
	}

	void FSystemStorage::Clear()
	{
		SystemsUnorderedMap.clear();
		SystemsWorkOrder.clear();
	}

	ISystem* FSystemStorage::Get(const TypeID_t SystemTypeID)
	{
		return SystemsUnorderedMap.at(SystemTypeID);
	}

	const ISystem* FSystemStorage::Get(const TypeID_t SystemTypeID) const
	{
		return SystemsUnorderedMap.at(SystemTypeID);
	}

	FSystemStorage::FSystemsVec& FSystemStorage::GetSystemsVec()
	{
		return SystemsWorkOrder;
	}

	const FSystemStorage::FSystemsVec& FSystemStorage::GetSystemsVec() const
	{
		return SystemsWorkOrder;
	}

	bool FSystemStorage::Contains(const TypeID_t SystemTypeID) const
	{
		FTypeIDSystemUnorderedMap::const_iterator It = SystemsUnorderedMap.find(SystemTypeID);

		return It != SystemsUnorderedMap.end() && It->second;
	}
}
