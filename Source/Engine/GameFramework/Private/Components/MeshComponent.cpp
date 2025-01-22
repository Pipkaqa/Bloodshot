#include "Components/MeshComponent.h"
#include "ResourceManager.h"

namespace Bloodshot
{
	FMeshComponent::FMeshComponent(FMesh&& Mesh)
		: Mesh(std::move(Mesh))
	{
	}

	FMeshComponent::FMeshComponent(const TVector<FVertex>& Vertices)
		: Mesh(Vertices)
	{
	}

	FMeshComponent::FMeshComponent(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices)
		: Mesh(Vertices, Indices)
	{
	}
}
