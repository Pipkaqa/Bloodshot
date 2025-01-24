#include "Serialization/Serializer.h"
#include "AssertionMacros.h"
#include "Templates/Containers/String.h"

namespace Bloodshot
{
	FSerializer::FSerializer(const std::filesystem::path& OutputPath)
		: OutputStream(OutputPath)
	{
	}

	void FSerializer::WriteLine(std::string_view String)
	{
		for (size_t i = 0; i < PushedScopes; ++i)
		{
			OutputStream << "\t";
		}

		OutputStream << String << "\n";
	}

	void FSerializer::EmptyLine()
	{
		OutputStream << "\n";
	}

	void FSerializer::PushScope()
	{
		++PushedScopes;
	}

	void FSerializer::PopScope()
	{
		BS_ASSERT(PushedScopes, "PopScope() called when PushedScopes == 0");
		--PushedScopes;
	}
}
