#include "MeshComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Vertex.h"

namespace Bloodshot
{
	FMeshComponent::FMeshComponent(const std::vector<FVertex>& Vertices)
	{
		VertexBuffer = FResourceManager::CreateVertexBuffer(Vertices);

		const FVertexBufferLayout& VertexBufferLayout =
		{
			{EShaderDataType::Float3, "in_Position"},
			{EShaderDataType::Float3, "in_Color"},
			{EShaderDataType::Float2, "in_TextureCoords"},
			{EShaderDataType::Float3, "in_Normal"},
			{EShaderDataType::Float3, "in_Tangent"}
		};

		VertexBuffer->SetLayout(VertexBufferLayout);

		VertexArray = FResourceManager::CreateVertexArray();

		VertexArray->AddVertexBuffer(VertexBuffer);
	}

	FMeshComponent::FMeshComponent(const std::vector<FVertex>& Vertices, const std::vector<uint32_t>& Indices)
		: FMeshComponent(Vertices)
	{
		VertexArray->Bind();

		IIndexBuffer* const IndexBuffer = FResourceManager::CreateIndexBuffer(Indices);

		VertexArray->SetIndexBuffer(IndexBuffer);
	}
}
