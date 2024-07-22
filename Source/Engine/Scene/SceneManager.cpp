#include "SceneManager.h"

#include "Debug/Logger.h"

#include <Rendering/Renderer.h>

namespace Bloodshot
{
	void SceneManager::BeginSimulation()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		m_ActiveScene->BeginSimulation();
	}

	void SceneManager::EndSimulation()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		m_ActiveScene->EndSimulation();
	}

	void SceneManager::BeginPlay()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		FL_CORE_DEBUG("Creating scene of type[{0}]...", m_ActiveScene->GetTypeName());

		m_ActiveScene->InternalBeginPlay();

		m_ActiveScene->BeginPlay();
	}

	void SceneManager::EndPlay()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		m_ActiveScene->EndPlay();

		m_ActiveScene->InternalEndPlay();
	}

	void SceneManager::InternalUpdate(float deltaTime, Renderer* renderer)
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		m_ActiveScene->InternalUpdate(deltaTime, renderer);
	}

	void SceneManager::Init()
	{
		FL_CORE_DEBUG("Creating scene manager...");
	}

	void SceneManager::Dispose()
	{
		FL_CORE_DEBUG("Destroying scene manager...");

		for (auto& [sceneTypeID, scene] : m_Scenes)
		{
			if (!scene) continue;

			FL_CORE_DEBUG("Destroying scene of type [{0}]...", scene->GetTypeName());

			scene->Dispose();

			scene = nullptr;
		}
	}
}
