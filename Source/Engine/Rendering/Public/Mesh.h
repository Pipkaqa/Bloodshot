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
		FMesh(const TVector<FVertex>& Vertices);
		FMesh(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices);

		void Rebuild(const TVector<FVertex>& Vertices);
		void Rebuild(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices);

		TUniquePtr<IVertexArray> VertexArray = nullptr;
		TVector<FSubMeshInfo> SubMeshInfoVec;
		TVector<FMaterial> MaterialVec;
	};
}
