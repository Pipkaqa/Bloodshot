#pragma once

#include "Platform/Platform.h"

int main(int argc, char** argv);

namespace Bloodshot
{
#define ENTRY_POINT_MODULE						                        \
friend int ::main(int argc, char** argv)                              

#define CORE_MODULE														\
friend class Core

#define ECS_MODULE														\
template<typename T>													\
friend class Component;													\
friend class ComponentManager;											\
friend class ComponentStorage;											\
friend class ECS;														\
template<typename T>													\
friend class Entity;													\
friend class EntityManager;												\
friend class EntityStorage;												\
friend class IComponent;												\
friend class IEntity;													\
friend class ISystem;													\
template<typename T>													\
friend class System;													\
friend class SystemManager;												\
friend class SystemStorage;												\
friend class Scene;														\
friend class SceneManager												

#define RENDERER_MODULE													\
friend class Renderer;													\
friend class OpenGLRenderer;											\
friend class Window;													\
friend class OpenGLWindow;												\
friend class Scene	   													

#define RESOURCE														\
friend class ResourceManager

#define BS_CHECK_ERROR(Expression, Format, ...)							\
if(!Expression)															\
{																		\
	BS_ERROR(Format, __VA_ARGS__);										\
}

#define BS_CHECK_ERROR_WITH_CALL(Expression, Format, FunctionCall, ...) \
if(!Expression)															\
{																		\
	BS_ERROR(Format, __VA_ARGS__);										\
	FunctionCall;														\
}

#define BS_CHECK_FATAL(Expression, Format, ...)							\
if(!Expression)															\
{																		\
	BS_FATAL(Format, __VA_ARGS__);										\
	TERMINATE;															\
}

#define BS_CHECK_FATAL_WITH_CALL(Expression, Format, FunctionCall, ...) \
if(!Expression)															\
{																		\
	BS_FATAL(Format, __VA_ARGS__);										\
	FunctionCall;														\
	TERMINATE;															\
}

	NODISCARD void* Malloc(const size_t size);

	void PrintGraphicsStatus();
	void PrintSceneStatus();
	void PrintECSStatus();
	void PrintMemoryStatus();
	void PrintEngineStatus();
}
