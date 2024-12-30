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
		FMeshComponent(const std::vector<FVertex>& Vertices);
		FMeshComponent(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices);

	private:
		FMesh Mesh;
	};
}
