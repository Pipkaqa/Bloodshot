#include "SceneManager.h"

#include "Debug/Logger.h"

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

	void SceneManager::InternalBeginPlay()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");
		FL_CORE_DEBUG("Begin playing on scene of type[{0}]...", m_ActiveScene->GetTypeName());

		m_ActiveScene->InternalBeginPlay();
		m_ActiveScene->BeginPlay();
		//m_ActiveScene->NoticeBeginPlay(); Notice all entities, components and systems about BeginPlay (in the future, when editor will be created)
	}

	void SceneManager::InternalEndPlay()
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");
		FL_CORE_DEBUG("End playing on scene of type[{0}]...", m_ActiveScene->GetTypeName());

		//m_ActiveScene->NoticeEndPlay(); Notice all entities, components and systems about EndPlay (in the future, when editor will be created)
		m_ActiveScene->EndPlay();
		m_ActiveScene->InternalEndPlay();
	}

	void SceneManager::InternalUpdate(float deltaTime, Renderer* renderer, Window* window)
	{
		FL_CORE_ASSERT(m_ActiveScene, "Active scene not found");

		m_ActiveScene->InternalUpdate(deltaTime, renderer, window);
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

			FL_CORE_TRACE("Destroying scene of type [{0}]...", scene->GetTypeName());

			delete scene;

			scene = nullptr;
		}

		m_Scenes.clear();
	}
}
