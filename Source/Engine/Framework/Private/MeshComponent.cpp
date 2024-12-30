#include "MeshComponent.h"
#include "ResourceManager.h"

namespace Bloodshot
{
	FMeshComponent::FMeshComponent(FMesh&& Mesh)
		: Mesh(std::move(Mesh))
	{
	}

	FMeshComponent::FMeshComponent(const std::vector<FVertex>& Vertices)
		: Mesh(Vertices)
	{
	}

	FMeshComponent::FMeshComponent(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices)
		: Mesh(Vertices, Indices)
	{
	}
}
