#include "EngineLoop.h"

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;

	Shared::FCmdParser CmdParser(Argc, Argv);
	FEngineLoop EngineLoop;

	EngineLoop.PreInit(CmdParser);
	EngineLoop.Init();
	EngineLoop.Run();
	EngineLoop.Exit();
	EngineLoop.Dispose();
}
