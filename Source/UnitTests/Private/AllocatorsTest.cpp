#include "Framework.h"

#include <BlockAllocator.h>
#include <Memory/Memory.h>

namespace
{
	static void BlockAllocatorTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		TBlockAllocator<double> DoubleAllocator(1024, 0);

		std::vector<double*> Doubles;
		Doubles.reserve(1024);

		for (size_t i = 0; i < 1024; ++i)
		{
			Doubles.emplace_back((double*)DoubleAllocator.Allocate(1));
		}

		FBlockAllocatorStats AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(1, AllocatorStats.ChunkCount);
		IAssert::AreEqual(1024, AllocatorStats.BlockCount);
		IAssert::AreEqual(0, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(1024, AllocatorStats.InUseBlockCount);

		DoubleAllocator.Reset();
		AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(1, AllocatorStats.ChunkCount);
		IAssert::AreEqual(1024, AllocatorStats.BlockCount);
		IAssert::AreEqual(1024, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(0, AllocatorStats.InUseBlockCount);

		DoubleAllocator.Dispose();
		AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(0, AllocatorStats.ChunkCount);
		IAssert::AreEqual(0, AllocatorStats.BlockCount);
		IAssert::AreEqual(0, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(0, AllocatorStats.InUseBlockCount);

		DoubleAllocator = std::move(TBlockAllocator<double>(64, 32));
		AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(32, AllocatorStats.ChunkCount);
		IAssert::AreEqual(64 * 32, AllocatorStats.BlockCount);
		IAssert::AreEqual(64 * 32, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(0, AllocatorStats.InUseBlockCount);

		Doubles.clear();
		for (size_t i = 0; i < 64 * 33; ++i)
		{
			Doubles.emplace_back((double*)DoubleAllocator.Allocate(1));
		}
		AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(33, AllocatorStats.ChunkCount);
		IAssert::AreEqual(64 * 33, AllocatorStats.BlockCount);
		IAssert::AreEqual(0, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(64 * 33, AllocatorStats.InUseBlockCount);

		for (size_t i = 0; i < 64 * 32; ++i)
		{
			DoubleAllocator.Deallocate((void*)Doubles[i], sizeof(Doubles.at(i)));
		}
		AllocatorStats = DoubleAllocator.GetStats();

		IAssert::AreEqual(33, AllocatorStats.ChunkCount);
		IAssert::AreEqual(64 * 33, AllocatorStats.BlockCount);
		IAssert::AreEqual(64 * 32, AllocatorStats.FreeBlockCount);
		IAssert::AreEqual(64 * 1, AllocatorStats.InUseBlockCount);
	}

	static void LinearTempAllocatorTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		FTempLinearAllocator LinearAllocator(1024 * sizeof(double));

		for (size_t i = 0; i < 1024; ++i)
		{
			LinearAllocator.Allocate(sizeof(double));
		}

		FTempLinearAllocatorStats AllocatorStats = LinearAllocator.GetStats();

		IAssert::AreEqual(8192, AllocatorStats.Size);
		IAssert::AreEqual(8192, AllocatorStats.Offset);

		LinearAllocator.Allocate(sizeof(double));
		AllocatorStats = LinearAllocator.GetStats();

		IAssert::AreEqual(8, AllocatorStats.Offset);

		LinearAllocator.Reset();
		AllocatorStats = LinearAllocator.GetStats();

		IAssert::AreEqual(0, AllocatorStats.Offset);

		LinearAllocator.Deallocate(nullptr, 0);
		AllocatorStats = LinearAllocator.GetStats();

		IAssert::AreEqual(0, AllocatorStats.Offset);

		LinearAllocator.Dispose();
		AllocatorStats = LinearAllocator.GetStats();

		IAssert::AreEqual(0, AllocatorStats.Size);
		IAssert::AreEqual(0, AllocatorStats.Offset);
	}
}

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;
	using namespace Bloodshot::Test;

#ifdef BS_LOGGING_ON
	FLogger Logger;
	Logger.BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
	BS_CHECK(Logger.IsSessionStarted(), "Logging Session not started!");
#endif
	{
		FMemory Memory;
		BlockAllocatorTest();
		LinearTempAllocatorTest();
	}
#ifdef BS_LOGGING_ON
	const FAllocationInfo& AllocationsInfo = IAllocationLogger::GetAllocationsInfo();

	BS_LOG(Info, "Allocated:   {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.AllocatedSize,
		AllocationsInfo.AllocatedSize >> 20ULL,
		AllocationsInfo.AllocatedBlockCount);

	BS_LOG(Info, "Deallocated: {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.DeallocatedSize,
		AllocationsInfo.DeallocatedSize >> 20ULL,
		AllocationsInfo.DeallocatedBlockCount);

	IAllocationLogger::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");
	Logger.EndSession();
#endif
}
