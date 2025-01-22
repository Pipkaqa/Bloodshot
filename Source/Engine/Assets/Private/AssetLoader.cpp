#include "AssetLoader.h"
#include "Assimp.h"
#include "Material.h"
#include "Mesh.h"
#include "OpenGL/OpenGLTexture.h"
#include "ResourceManager.h"
#include "StbImage.h"
#include "Texture.h"
#include "Vertex.h"

namespace Bloodshot
{
	static TUniquePtr<ITexture> InternalCreateTexture(const aiTextureType Type,
		const aiScene* const Scene,
		const aiMaterial* const Material,
		const bool bFlipped = false)
	{
		using namespace Bloodshot;

		if (Material->GetTextureCount(Type) > 0)
		{
			aiString TexturePath;

			const aiReturn Result = Material->GetTexture(Type,
				0,
				&TexturePath,
				nullptr,
				nullptr,
				nullptr,
				nullptr,
				nullptr);

			if (Result == AI_SUCCESS)
			{
				const aiTexture* const EmbeddedTexture = Scene->GetEmbeddedTexture(TexturePath.C_Str());

				if (EmbeddedTexture)
				{
					return FResourceManager::CreateTextureFromMemory(EmbeddedTexture->pcData, EmbeddedTexture->mWidth);
				}
				else
				{
					return FResourceManager::CreateTexture(TexturePath.C_Str(), bFlipped);
				}
			}
		}

		return nullptr;
	}

	static void LoadSubMesh(const aiScene* const Scene,
		const aiMesh* const SubMesh,
		FMesh& OutMesh,
		TVector<FVertex>& OutVertexVec,
		TVector<uint32_t>& OutIndexVec,
		uint32_t& OutMeshVertexCount,
		uint32_t& OutMeshIndexCount)
	{
		using namespace Bloodshot;

		FSubMeshInfo& SubMeshInfo = OutMesh.SubMeshInfoVec.emplace_back();

		SubMeshInfo.StartVertex = OutMeshVertexCount;

		const uint32_t SubMeshVertexCount = SubMesh->mNumVertices;

		OutMeshVertexCount += SubMeshVertexCount;

		SubMeshInfo.StartIndex = OutMeshIndexCount;

		const uint32_t SubMeshIndexCount = SubMesh->mNumFaces * 3;

		OutMeshIndexCount += SubMeshIndexCount;

		SubMeshInfo.IndexCount = SubMeshIndexCount;

		for (size_t i = 0; i < SubMeshVertexCount; ++i)
		{
			const aiVector3D& Position = SubMesh->mVertices[i];
			const aiVector3D& TextureCoords = SubMesh->HasTextureCoords(0) ? SubMesh->mTextureCoords[0][i] : aiVector3D(0.f);
			const aiVector3D& Normals = SubMesh->mNormals[i];

			OutVertexVec.emplace_back(glm::vec3(Position.x, Position.y, Position.z),
				glm::vec3(0.f),
				glm::vec2(TextureCoords.x, TextureCoords.y),
				glm::vec3(Normals.x, Normals.y, Normals.z),
				glm::vec3(0.f));
		}

		for (size_t i = 0; i < SubMesh->mNumFaces; ++i)
		{
			const aiFace& Face = SubMesh->mFaces[i];
			const uint32_t* const IndexArr = Face.mIndices;

			OutIndexVec.push_back(IndexArr[0]);
			OutIndexVec.push_back(IndexArr[1]);
			OutIndexVec.push_back(IndexArr[2]);
		}

		SubMeshInfo.MaterialIndex = SubMesh->mMaterialIndex;
	}

	static void LoadSubMeshesFromNodeRecursive(const aiScene* const Scene,
		const aiNode* const Node,
		FMesh& OutMesh,
		TVector<FVertex>& OutVertexVec,
		TVector<uint32_t>& OutIndexVec,
		uint32_t& OutMeshVertexCount,
		uint32_t& OutMeshIndexCount)
	{
		for (size_t i = 0; i < Node->mNumMeshes; ++i)
		{
			LoadSubMesh(Scene,
				Scene->mMeshes[Node->mMeshes[i]],
				OutMesh,
				OutVertexVec,
				OutIndexVec,
				OutMeshVertexCount,
				OutMeshIndexCount);
		}

		for (size_t i = 0; i < Node->mNumChildren; ++i)
		{
			LoadSubMeshesFromNodeRecursive(Scene,
				Node->mChildren[i],
				OutMesh,
				OutVertexVec,
				OutIndexVec,
				OutMeshVertexCount,
				OutMeshIndexCount);
		}
	}

	static glm::vec3 LoadColor(const aiMaterial* const Material, const char* Key, const uint32_t Type, const uint32_t Index)
	{
		aiColor3D Color;

		return Material->Get(Key, Type, Index, Color) == AI_SUCCESS ? glm::vec3(Color.r, Color.g, Color.b) : glm::vec3(0.f);
	}

	static void LoadMaterials(const aiScene* const Scene, FMesh& OutMesh)
	{
		using namespace Bloodshot;

		for (size_t i = 0; i < Scene->mNumMaterials; ++i)
		{
			FMaterial& Material = OutMesh.MaterialVec.emplace_back();

			const aiMaterial* const SubMeshMaterial = Scene->mMaterials[i];

			Material.AlbedoTexture = InternalCreateTexture(aiTextureType_BASE_COLOR, Scene, SubMeshMaterial);
			Material.RoughnessTexture = InternalCreateTexture(aiTextureType_DIFFUSE_ROUGHNESS, Scene, SubMeshMaterial);
			Material.MetallicTexture = InternalCreateTexture(aiTextureType_METALNESS, Scene, SubMeshMaterial);
			Material.NormalMapTexture = InternalCreateTexture(aiTextureType_HEIGHT, Scene, SubMeshMaterial);
			Material.DiffuseTexture = InternalCreateTexture(aiTextureType_DIFFUSE, Scene, SubMeshMaterial);
			Material.SpecularTexture = InternalCreateTexture(aiTextureType_SHININESS, Scene, SubMeshMaterial);

			Material.AmbientColor = LoadColor(SubMeshMaterial, AI_MATKEY_COLOR_AMBIENT);
			Material.DiffuseColor = LoadColor(SubMeshMaterial, AI_MATKEY_COLOR_DIFFUSE);
			Material.SpecularColor = LoadColor(SubMeshMaterial, AI_MATKEY_COLOR_SPECULAR);
		}
	}

	FMesh IAssetLoader::LoadMeshFromFile(FStringView Path)
	{
		Assimp::Importer AssetImporter;

		const aiScene* const Scene = AssetImporter.ReadFile(Path.data(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		if (!Scene)
		{
			BS_LOG(Error, "Error occured while loading mesh from file: {0}", AssetImporter.GetErrorString());
			return FMesh();
		}

		FMesh Mesh;

		Mesh.SubMeshInfoVec.reserve(Scene->mNumMeshes);
		Mesh.MaterialVec.reserve(Scene->mNumMaterials);

		TVector<FVertex> VertexVec;
		TVector<uint32_t> IndexVec;

		uint32_t MeshVertexCount = 0;
		uint32_t MeshIndexCount = 0;

		LoadSubMeshesFromNodeRecursive(Scene, Scene->mRootNode, Mesh, VertexVec, IndexVec, MeshVertexCount, MeshIndexCount);
		LoadMaterials(Scene, Mesh);

		Mesh.Rebuild(VertexVec, IndexVec);

		return Mesh;
	}
}
