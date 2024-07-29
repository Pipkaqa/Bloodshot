#include "SceneManager.h"

namespace Bloodshot
{
	void SceneManager::BeginSimulation()
	{
		BS_ASSERT(m_CurrentScene, "Active scene not found");

		m_CurrentScene->BeginSimulation();
	}

	void SceneManager::EndSimulation()
	{
		BS_ASSERT(m_CurrentScene, "Active scene not found");

		m_CurrentScene->EndSimulation();
	}

	void SceneManager::InternalBeginPlay()
	{
		BS_ASSERT(m_CurrentScene, "Active scene not found");
		BS_DEBUG("Begin playing on scene of type[{0}]...", m_CurrentScene->GetTypeName());

		m_CurrentScene->InternalBeginPlay();
		m_CurrentScene->BeginPlay();
		//m_CurrentScene->NoticeBeginPlay(); Notice all entities, components and systems about BeginPlay (in the future, when editor will be created)
	}

	void SceneManager::InternalEndPlay()
	{
		BS_ASSERT(m_CurrentScene, "Active scene not found");
		BS_DEBUG("End playing on scene of type[{0}]...", m_CurrentScene->GetTypeName());

		//m_CurrentScene->NoticeEndPlay(); Notice all entities, components and systems about EndPlay (in the future, when editor will be created)
		m_CurrentScene->EndPlay();
		m_CurrentScene->InternalEndPlay();
	}

	void SceneManager::InternalUpdate(float deltaTime, Renderer* renderer, Window* window)
	{
		BS_ASSERT(m_CurrentScene, "Active scene not found");

		m_CurrentScene->InternalUpdate(deltaTime, renderer, window);
	}

	void SceneManager::Init()
	{
		BS_DEBUG("Creating scene manager...");
	}

	void SceneManager::Dispose()
	{
		BS_DEBUG("Destroying scene manager...");

		for (auto& [sceneTypeID, scene] : m_Scenes)
		{
			if (!scene) continue;

			BS_TRACE("Destroying scene of type [{0}]...", scene->GetTypeName());

			delete scene;
		}

		m_Scenes.clear();
	}
}
