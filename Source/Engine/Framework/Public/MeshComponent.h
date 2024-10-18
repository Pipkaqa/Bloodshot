#pragma once

#include "Component.h"
#include "MathLibrary.h"
#include "Platform.h"

#include <cstdint>
#include <vector>

namespace Bloodshot
{
	namespace Private
	{
		struct FRenderingSystem;
	}

	struct FVertex;
	class IVertexArray;
	class IVertexBuffer;
	class IIndexBuffer;

	class FMeshComponent final : public IComponent
	{
		friend struct Private::FRenderingSystem;

	public:
		FMeshComponent(const std::vector<FVertex>& Vertices);
		FMeshComponent(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices);

	private:
		IVertexArray* VertexArray = nullptr;
		IVertexBuffer* VertexBuffer = nullptr;
		IIndexBuffer* IndexBuffer = nullptr;
	};
}
