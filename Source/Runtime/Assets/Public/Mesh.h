#pragma once

#include "Core.h"

#include "Material.h"
#include "VertexArray.h"

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
		FMesh(const TArray<FVertex>& Vertices);
		FMesh(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices);

		void Rebuild(const TArray<FVertex>& Vertices);
		void Rebuild(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices);

		TUniquePtr<IVertexArray> VertexArray = nullptr;
		TArray<FSubMeshInfo> SubMeshInfos;
		TArray<FMaterial> Materials;
	};
}
