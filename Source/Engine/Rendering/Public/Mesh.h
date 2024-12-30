#pragma once

#include "Material.h"
#include "MathLibrary.h"
#include "Platform.h"
#include "Templates/SmartPointers.h"
#include "VertexArray.h"

#include <cstdint>
#include <vector>

namespace Bloodshot
{
	struct FVertex;

	struct FSubMeshInfo final
	{
		uint32_t StartVertex;
		uint32_t StartIndex;
		uint32_t IndexCount;
		uint32_t MaterialIndex;
	};

	struct FMesh final
	{
		FMesh() {}
		FMesh(const std::vector<FVertex>& Vertices);
		FMesh(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices);

		void Rebuild(const std::vector<FVertex>& Vertices);
		void Rebuild(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices);

		TUniquePtr<IVertexArray> VertexArray = nullptr;
		std::vector<FSubMeshInfo> SubMeshInfoVec;
		std::vector<FMaterial> MaterialVec;
	};
}
