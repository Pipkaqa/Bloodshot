#include "Components/MeshComponent.h"
#include "ResourceManager.h"
#include "Vertex.h"

namespace Bloodshot
{
	FMeshComponent::FMeshComponent(FMesh&& Mesh)
		: Mesh(std::move(Mesh))
	{
	}

	FMeshComponent::FMeshComponent(const TArray<FVertex>& Vertices)
		: Mesh(Vertices)
	{
	}

	FMeshComponent::FMeshComponent(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices)
		: Mesh(Vertices, Indices)
	{
	}
}
