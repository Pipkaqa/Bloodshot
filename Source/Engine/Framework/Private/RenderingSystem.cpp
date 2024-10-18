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
		// BSTODO: Finish implementation

		using FMeshComponentIterator = TComponentIterator<FMeshComponent>;

		FCameraComponent* const CameraComponent = FSceneManager::GetCurrentScene()->MainCameraComponent;
		FEntity* const Camera = CameraComponent->GetOwner();
		FTransformComponent* const CameraTransformComponent = IECS::GetComponent<FTransformComponent>(Camera);

		const glm::mat4& IdentityMatrix = glm::mat4(1.f);
		const glm::mat4& ViewMatrix = CameraComponent->GetViewMatrix();
		const glm::mat4& ProjectionMatrix = CameraComponent->GetProjectionMatrix();

		IShader* const Shader = Renderer->DefaultShader;

		FMeshComponentIterator MeshComponentIterator = IECS::GetBeginComponentIterator<FMeshComponent>();
		FMeshComponentIterator MeshComponentEndIterator = IECS::GetEndComponentIterator<FMeshComponent>();

		for (; MeshComponentIterator != MeshComponentEndIterator; ++MeshComponentIterator)
		{
			//const glm::mat4& ModelMatrix = glm::translate(IdentityMatrix, glm::vec3(0.f))
			//	* glm::rotate(IdentityMatrix, glm::radians(Rotation), FVector3::Right)
			//	* glm::rotate(IdentityMatrix, glm::radians(Rotation), FVector3::Up)
			//	* glm::rotate(IdentityMatrix, glm::radians(Rotation), FVector3::Forward)
			//	* glm::scale(IdentityMatrix, glm::vec3(1.f));

			const glm::mat4& ModelMatrix = IdentityMatrix;

			Shader->SetUniformMat4("uni_ModelViewProjectionMatrix", ModelMatrix * ViewMatrix * ProjectionMatrix);
			Shader->Bind();

			IVertexArray* const VertexArray = MeshComponentIterator->VertexArray;

			if (VertexArray->GetIndexBuffer())
			{
				Renderer->DrawIndexed(VertexArray);
			}
			else
			{
				Renderer->DrawTriangles(VertexArray);
			}
		}
	}
}
