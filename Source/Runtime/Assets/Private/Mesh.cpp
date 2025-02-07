#include "Mesh.h"
#include "IndexBuffer.h"
#include "ResourceManager.h"
#include "Vertex.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

namespace Bloodshot
{
	FMesh::FMesh(const TArray<FVertex>& Vertices)
		: VertexArray(IResourceManager::CreateVertexArray())
	{
		TUniquePtr<IVertexBuffer> VertexBuffer = IResourceManager::CreateVertexBuffer(Vertices);

		// BSTODO: Layout std::move

		FVertexBufferLayout Layout =
		{
			{EShaderDataType::Float3, "in_Position"},
			{EShaderDataType::Float3, "in_Color"},
			{EShaderDataType::Float2, "in_TextureCoords"},
			{EShaderDataType::Float3, "in_Normal"},
			{EShaderDataType::Float3, "in_Tangent"}
		};

		VertexBuffer->SetLayout(Layout);
		VertexArray->AddVertexBuffer(std::move(VertexBuffer));
	}

	FMesh::FMesh(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices)
		: FMesh(Vertices)
	{
		VertexArray->Bind();
		VertexArray->SetIndexBuffer(IResourceManager::CreateIndexBuffer(Indices));
	}

	void FMesh::Rebuild(const TArray<FVertex>& Vertices)
	{
		VertexArray = IResourceManager::CreateVertexArray();

		TUniquePtr<IVertexBuffer> VertexBuffer = IResourceManager::CreateVertexBuffer(Vertices);

		// BSTODO: Layout std::move

		FVertexBufferLayout Layout =
		{
			{EShaderDataType::Float3, "in_Position"},
			{EShaderDataType::Float3, "in_Color"},
			{EShaderDataType::Float2, "in_TextureCoords"},
			{EShaderDataType::Float3, "in_Normal"},
			{EShaderDataType::Float3, "in_Tangent"}
		};

		VertexBuffer->SetLayout(Layout);
		VertexArray->AddVertexBuffer(std::move(VertexBuffer));
	}

	void FMesh::Rebuild(const TArray<FVertex>& Vertices, const TArray<uint32_t>& Indices)
	{
		Rebuild(Vertices);

		VertexArray->Bind();
		VertexArray->SetIndexBuffer(IResourceManager::CreateIndexBuffer(Indices));
	}
}
