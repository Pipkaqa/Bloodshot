#pragma once

#include "EngineFramework.h"
#include "Utility/ISingleton.h"

#include <chrono>

namespace Bloodshot
{
	class EngineTime final : ISingleton<EngineTime>
	{
		CORE_MODULE;

	public:
		NODISCARD FORCEINLINE static float Global()
		{
			return s_Instance->m_GlobalTime;
		}

	private:
		using ISingleton::Create;

		std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();
		float m_GlobalTime = 0.f;

		void Init() override;
		void Dispose() override;

		void Tick();
	};
}
