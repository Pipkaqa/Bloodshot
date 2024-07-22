#pragma once

namespace Bloodshot
{
#define OWNED_BY_CORE		   \
friend class Core

#define ECS_PART			   \
template<typename T>		   \
friend class Component;		   \
friend class ComponentManager; \
friend class ComponentStorage; \
friend class ECS;			   \
template<typename T>		   \
friend class Entity;		   \
friend class EntityManager;	   \
friend class EntityStorage;	   \
friend class IComponent;	   \
friend class IEntity;		   \
friend class ISystem;		   \
template<typename T>		   \
friend class System;		   \
friend class SystemManager;    \
friend class SystemStorage     

#define RENDERING_PART		   \
friend class Renderer;		   \
friend class OpenGLRenderer;   \
friend class Window;		   \
friend class OpenGLWindow;	   \

#define RESOURCE			   \
friend class ResourceManager;
}
