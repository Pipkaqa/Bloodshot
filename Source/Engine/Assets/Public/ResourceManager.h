#pragma once

#include "MathLibrary.h"
#include "Platform.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

#include <list>
#include <unordered_map>

namespace Bloodshot
{
	struct FVertex;
	class IVertexArray;
	class IVertexBuffer;
	class IIndexBuffer;
	class IUniformBuffer;
	struct FFrameBufferSettings;
	class IFrameBuffer;
	class IShader;
	class ITexture;
	struct FMaterial;

	class FResourceManager final : public TSingleton<FResourceManager>
	{
	public:
		using FNameShaderTable = std::unordered_map<std::string, TReference<IShader>>;

		FResourceManager();

		FNameShaderTable ShadersTable;

		virtual void Init() override;
		virtual void Dispose() override;

		static TUniquePtr<IVertexArray> CreateVertexArray();
		static TUniquePtr<IVertexBuffer> CreateVertexBuffer(const std::vector<FVertex>& Vertices);
		static TUniquePtr<IIndexBuffer> CreateIndexBuffer(const std::vector<uint32_t>& Indices);
		static TUniquePtr<IUniformBuffer> CreateUniformBuffer(const uint32_t Size, const uint32_t Binding);
		static TUniquePtr<IFrameBuffer> CreateFrameBuffer(const FFrameBufferSettings& Settings);
		static TUniquePtr<IShader> CreateShader(std::string_view Name,
			std::string_view VertexShaderSrc,
			std::string_view FragmentShaderSrc);
		static TUniquePtr<IShader> CreateShaderFromFile(std::string_view Name,
			std::string_view VertexShaderPath,
			std::string_view FragmentShaderPath);
		static TUniquePtr<ITexture> CreateTexture(std::string_view Path, const bool bFlipped);
		static TUniquePtr<ITexture> CreateTextureFromMemory(const void* const Data, const uint32_t DataSize);
	};
}
