#pragma once

//TODO: Optimize

#include "Benchmark/Timer.h"

#include "Core/Assert.h"
#include "Core/Core.h"
#include "Core/EngineFramework.h"
#include "Core/EngineState.h"
#include "Core/Locale.h"
#include "Core/ResourceManager.h"

#include "Debug/Logger.h"

#include "ECS/Component.h"
#include "ECS/ComponentManager.h"
#include "ECS/ComponentStorage.h"
#include "ECS/ECS.h"
#include "ECS/Entity.h"
#include "ECS/EntityManager.h"
#include "ECS/EntityStorage.h"
#include "ECS/IComponent.h"
#include "ECS/IEntity.h"
#include "ECS/ISystem.h"
#include "ECS/System.h"
#include "ECS/SystemManager.h"
#include "ECS/SystemStorage.h"

#include "GameFramework/Components.h"
#include "GameFramework/World.h"

#include "Math/Math.h"

#include "Memory/FixedAllocator.h"
#include "Memory/LinearAllocator.h"
#include "Memory/MemoryManager.h"

//#include "Physics/"

#include "Platform/OpenGL/OpenGLHeader.h"
#include "Platform/Platform.h"

//#include "Project/"

#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Rendering/Window.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "Test/TestFramework.h"

#include "Utility/ISingleton.h"
#include "Utility/TypeInfo.h"
#include "Utility/Utility.h"
