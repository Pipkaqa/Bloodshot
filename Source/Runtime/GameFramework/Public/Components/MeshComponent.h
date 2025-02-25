#pragma once

#include "Core.h"

#include "Component.h"
#include "Mesh.h"

namespace Bloodshot
{
	namespace Private::Rendering { class FRenderingSystem; }

	class FMeshComponent final : public IComponent
	{
		friend class Private::Rendering::FRenderingSystem;

	public:
		FORCEINLINE FMeshComponent(FMesh&& Mesh)
			: Mesh(std::move(Mesh))
		{
		}

		FORCEINLINE FMeshComponent(const TArray<FVertex>& Vertices)
			: Mesh(Vertices)
		{
		}

		FORCEINLINE FMeshComponent(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices)
			: Mesh(Vertices, Indices)
		{
		}

	private:
		FMesh Mesh;
	};
}
