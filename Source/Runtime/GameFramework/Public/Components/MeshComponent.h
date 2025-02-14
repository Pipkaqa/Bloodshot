#pragma once

#include "Core.h"

#include "Component.h"
#include "Mesh.h"

namespace Bloodshot
{
	namespace Private
	{
		class FRenderingSystem;
	}

	class FMeshComponent final : public IComponent
	{
		friend class Private::FRenderingSystem;

	public:
		FMeshComponent(FMesh&& Mesh);
		FMeshComponent(const TArray<FVertex>& Vertices);
		FMeshComponent(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices);

	private:
		FMesh Mesh;
	};
}
