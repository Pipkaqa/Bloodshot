#include "Systems/RenderingSystem.h"
#include "CameraComponent.h"
#include "ComponentManager.h"
#include "Components/MeshComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Shader.h"
#include "TransformComponent.h"
#include "VertexArray.h"

namespace Bloodshot::Private::Rendering
{
	void FRenderingSystem::Execute(float DeltaTime, TReference<IRenderer> Renderer)
	{
		BS_PROFILE_FUNCTION();
		TReference<FScene> Scene = FSceneManager::GetCurrentScene();
		if (!Scene) return;
		TReference<FCameraComponent> CameraComponent = Scene->MainCameraComponent;
		if (!CameraComponent) return;

		const glm::mat4& IdentityMatrix = {1.f};
		const glm::mat4& ViewMatrix = CameraComponent->GetViewMatrix();
		const glm::mat4& ProjectionMatrix = CameraComponent->GetProjectionMatrix();

		using FMeshComponentIterator = TComponentIterator<FMeshComponent>;
		FMeshComponentIterator MeshComponentIterator = FComponentManager::CreateComponentIterator<FMeshComponent>();
		//TUniquePtr<IShader>& Shader = Renderer->DefaultShader;

		for (; MeshComponentIterator; ++MeshComponentIterator)
		{
			const FMesh& Mesh = MeshComponentIterator->Mesh;
			const TReference<IVertexArray> VertexArray = Mesh.VertexArray.GetReference();
			if (!VertexArray) return;

			const TReference<FEntity> Owner = MeshComponentIterator->GetOwner();
			const TReference<FTransformComponent> MeshTransformComponent = FComponentManager::GetComponent<FTransformComponent>(Owner);

			const glm::vec3& MeshRotation = MeshTransformComponent->GetRotation();
			const glm::mat4& ModelMatrix = glm::translate(IdentityMatrix, MeshTransformComponent->GetPosition())
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.x - 90.f), IVector3Constants::Right)
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.y), IVector3Constants::Forward)
				* glm::rotate(IdentityMatrix, glm::radians(MeshRotation.z), IVector3Constants::Up)
				* glm::scale(IdentityMatrix, MeshTransformComponent->GetScale());

			//Shader->SetUniformMat4("uni_ModelViewProjectionMatrix", ProjectionMatrix * ViewMatrix * ModelMatrix);
			//Shader->Bind();

			const TArray<FSubMeshInfo>& SubMeshInfos = Mesh.SubMeshInfos;
			const TArray<FMaterial>& Materials = Mesh.Materials;
			const size_t SubMeshCount = SubMeshInfos.GetSize();

			if (SubMeshCount)
			{
				for (size_t i = 0; i < SubMeshCount; ++i)
				{
					const FSubMeshInfo& SubMeshInfo = SubMeshInfos[i];
					const uint32_t MaterialIndex = SubMeshInfo.MaterialIndex;
					const FMaterial& Material = Materials[MaterialIndex];
					if (Material.AlbedoTexture) Material.AlbedoTexture->Bind(ETextureUnit::Albedo);
					if (Material.RoughnessTexture) Material.RoughnessTexture->Bind(ETextureUnit::Roughness);
					if (Material.MetallicTexture) Material.MetallicTexture->Bind(ETextureUnit::Metallic);
					if (Material.NormalMapTexture) Material.NormalMapTexture->Bind(ETextureUnit::Normal);
					if (Material.DiffuseTexture) Material.DiffuseTexture->Bind(ETextureUnit::Color);
					if (Material.SpecularTexture) Material.SpecularTexture->Bind(ETextureUnit::Specular);
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
