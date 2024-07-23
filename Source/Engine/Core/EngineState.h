#pragma once

#include "EngineFramework.h"
#include "Utility/ISingleton.h"

namespace Bloodshot
{
	class EngineState final : public ISingleton<EngineState>
	{
		OWNED_BY_CORE;

	public:
		NODISCARD FORCEINLINE static bool Running()
		{
			return EngineState::s_Instance->m_Running;
		}

		NODISCARD FORCEINLINE static bool Simulating()
		{
			return EngineState::s_Instance->m_Simulating;
		}

	private:
		using ISingleton::Create;

		bool m_Running = false;
		bool m_Simulating = false;

		void Init() override {}
		void Dispose() override {}
	};
}
