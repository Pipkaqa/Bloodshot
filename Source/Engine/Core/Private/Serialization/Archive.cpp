#include "Serialization/Archive.h"
#include "AssertionMacros.h"

namespace Bloodshot
{
	FArchive::FArchive(const std::filesystem::path& OutputPath)
		: OutputStream(OutputPath)
	{
	}

	FArchive::~FArchive()
	{
		if (Buffer.size()) Flush();
		OutputStream.close();
	}

	void FArchive::Flush()
	{
		OutputStream << Buffer;
		Buffer.clear();
	}

	FArchive::EWriteState FArchive::GetWriteState() const
	{
		return WriteStateStack.front();
	}

	void FArchive::PushScope()
	{
		++PushedScopes;
	}

	void FArchive::PopScope()
	{
		BS_ASSERT(PushedScopes, "PopScope() called when PushedScopes == 0");
		--PushedScopes;
	}

	void FArchive::PushWriteState(const EWriteState WriteState)
	{
		WriteStateStack.push_front(WriteState);
	}

	void FArchive::PopWriteState()
	{
		WriteStateStack.pop_front();
	}

	void FArchive::PopBack()
	{
		Buffer.pop_back();
	}
}
