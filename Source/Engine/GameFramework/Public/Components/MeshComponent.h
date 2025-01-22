#pragma once

#include "Component.h"
#include "Mesh.h"
#include "Templates/SmartPointers.h"

#include <cstdint>
#include <vector>

namespace Bloodshot
{
	namespace Private
	{
		struct FRenderingSystem;
	}

	class FMeshComponent final : public IComponent
	{
		friend struct Private::FRenderingSystem;
		friend class FResourceManager;

	public:
		FMeshComponent(FMesh&& Mesh);
		FMeshComponent(const TVector<FVertex>& Vertices);
		FMeshComponent(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices);

	private:
		FMesh Mesh;
	};
}
