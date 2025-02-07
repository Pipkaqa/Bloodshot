#pragma once

#include "Containers/String.h"
#include "Containers/Array.h"

namespace Bloodshot
{
	class FProperty final
	{
	public:
		FString Type;
		FString Name;

		bool bStatic = false;
		bool bSerialized = false;
		bool bReplicated = false;
	};

	class FParameter final
	{
	public:
		FString Type;
		FString Name;
	};

	class FFunction final
	{
	public:
		FString ReturnType;
		FString Name;
		TArray<FParameter> Parameters;

		bool bStatic = false;
		bool bConst = false;
		bool bNoexcept = false;
	};

	class FClass final
	{
	public:
		FString Name;
		TArray<FClass> BaseClasses;
		TArray<FProperty> Properties;
		TArray<FFunction> Functions;

		bool bAbstract = false;
		bool bFinal = false;
		bool bDerived = false;

		size_t GetHash() const;
	};
}
