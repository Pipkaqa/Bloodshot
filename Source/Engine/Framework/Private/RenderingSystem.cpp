#include "RenderingSystem.h"
#include "CameraComponent.h"
#include "ECS.h"
#include "MathLibrary.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "VertexArray.h"

namespace Bloodshot::Private
{
	void FRenderingSystem::Execute(float DeltaTime, TUniquePtr<IRenderer>& Renderer)
	{
		using FMeshComponentIterator = TComponentIterator<FMeshComponent>;

		TReference<FCameraComponent> CameraComponent = FSceneManager::GetCurrentScene()->MainCameraComponent;

		if (!CameraComponent) return;

		const glm::mat4& ViewMatrix = CameraComponent->GetViewMatrix();
		const glm::mat4& ProjectionMatrix = CameraComponent->GetProjectionMatrix();

		TUniquePtr<IShader>& Shader = Renderer->DefaultShader;

		FMeshComponentIterator MeshComponentIterator = IECS::GetComponentBeginIterator<FMeshComponent>();
		FMeshComponentIterator MeshComponentEndIterator = IECS::GetComponentEndIterator<FMeshComponent>();

		const glm::mat4& IdentityMatrix = {1.f};

		for (; MeshComponentIterator != MeshComponentEndIterator; ++MeshComponentIterator)
		{
			FMesh& Mesh = MeshComponentIterator->Mesh;

			const TUniquePtr<IVertexArray>& VertexArray = Mesh.VertexArray;

			if (!VertexArray) return;

			TReference<FEntity> Owner = MeshComponentIterator->GetOwner();
			TReference<FTransformComponent> MeshTransformComponent = IECS::GetComponent<FTransformComponent>(Owner);
			const glm::vec3& MeshRotation = MeshTransformComponent->Rotation;

			const glm::mat4& ModelMatrix = glm::translate(IdentityMatrix, MeshTransformComponent->Position)
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.x - 90.f), IVector3Constants::Right)
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.y), IVector3Constants::Forward)
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.z), IVector3Constants::Up)
				* glm::scale(IdentityMatrix, MeshTransformComponent->Scale);

			Shader->SetUniformMat4("uni_ModelViewProjectionMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);
			Shader->Bind();

			const std::vector<FSubMeshInfo>& SubMeshInfoVec = Mesh.SubMeshInfoVec;
			const std::vector<FMaterial>& MaterialVec = Mesh.MaterialVec;

			const size_t SubMeshCount = SubMeshInfoVec.size();

			if(SubMeshCount)
			{
				for (size_t i = 0; i < SubMeshCount; ++i)
				{
					const FSubMeshInfo& SubMeshInfo = SubMeshInfoVec[i];

					const uint32_t MaterialIndex = SubMeshInfo.MaterialIndex;

					const FMaterial& Material = MaterialVec[MaterialIndex];

					if (Material.AlbedoTexture)
					{
						Material.AlbedoTexture->Bind(ETextureUnit::Albedo);
					}

					if (Material.RoughnessTexture)
					{
						Material.RoughnessTexture->Bind(ETextureUnit::Roughness);
					}

					if (Material.MetallicTexture)
					{
						Material.MetallicTexture->Bind(ETextureUnit::Metallic);
					}

					if (Material.NormalMapTexture)
					{
						Material.NormalMapTexture->Bind(ETextureUnit::Normal);
					}
					
					if (Material.DiffuseTexture)
					{
						Material.DiffuseTexture->Bind(ETextureUnit::Color);
					}

					if (Material.SpecularTexture)
					{
						Material.SpecularTexture->Bind(ETextureUnit::Specular);
					}

					Renderer->DrawPart(VertexArray, SubMeshInfo);
				}
			}
			else
			{
				VertexArray->GetIndexBuffer() ? Renderer->DrawIndexed(VertexArray) : Renderer->DrawTriangles(VertexArray);
			}
		}
	}
}
