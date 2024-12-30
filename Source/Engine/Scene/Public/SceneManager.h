#pragma once

#include "AssertionMacros.h"
#include "EngineState.h"
#include "Logging/LoggingMacros.h"
#include "Platform.h"
#include "Scene.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

#include <unordered_map>

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final : public TSingleton<FSceneManager>
	{
		friend class IECS;

	public:
		using FTypeIDSceneUnorderedMap = std::unordered_map<InstanceID_t, FScene>;

		FSceneManager();

		FTypeIDSceneUnorderedMap ScenesUnorderedMap;

		TReference<FScene> CurrentScene = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD FORCEINLINE static FScene* GetCurrentScene()
		{
			return Instance->CurrentScene;
		}

		static void LoadScene(const InstanceID_t Idx);

		void AddScene();
		void SetStartingScene(const InstanceID_t Idx);

		void BeginPlay();
		void EndPlay();

		void Tick(float DeltaTime);
	};
}
