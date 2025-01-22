#include "Mesh.h"
#include "IndexBuffer.h"
#include "ResourceManager.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

namespace Bloodshot
{
	FMesh::FMesh(const TVector<FVertex>& Vertices)
		: VertexArray(FResourceManager::CreateVertexArray())
	{
		TUniquePtr<IVertexBuffer> VertexBuffer = FResourceManager::CreateVertexBuffer(Vertices);
		VertexBuffer->SetLayout({{EShaderDataType::Float3, "in_Position"},
						 {EShaderDataType::Float3, "in_Color"},
						 {EShaderDataType::Float2, "in_TextureCoords"},
						 {EShaderDataType::Float3, "in_Normal"},
						 {EShaderDataType::Float3, "in_Tangent"}});

		VertexArray->AddVertexBuffer(std::move(VertexBuffer));
	}

	FMesh::FMesh(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices)
		: FMesh(Vertices)
	{
		VertexArray->Bind();
		VertexArray->SetIndexBuffer(FResourceManager::CreateIndexBuffer(Indices));
	}

	void FMesh::Rebuild(const TVector<FVertex>& Vertices)
	{
		VertexArray = FResourceManager::CreateVertexArray();

		TUniquePtr<IVertexBuffer> VertexBuffer = FResourceManager::CreateVertexBuffer(Vertices);
		VertexBuffer->SetLayout({{EShaderDataType::Float3, "in_Position"},
						 {EShaderDataType::Float3, "in_Color"},
						 {EShaderDataType::Float2, "in_TextureCoords"},
						 {EShaderDataType::Float3, "in_Normal"},
						 {EShaderDataType::Float3, "in_Tangent"}});

		VertexArray->AddVertexBuffer(std::move(VertexBuffer));
	}

	void FMesh::Rebuild(const TVector<FVertex>& Vertices, const TVector<uint32_t>& Indices)
	{
		Rebuild(Vertices);

		VertexArray->Bind();
		VertexArray->SetIndexBuffer(FResourceManager::CreateIndexBuffer(Indices));
	}
}
