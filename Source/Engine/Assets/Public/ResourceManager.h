#pragma once

#include "Core.h"

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

	// BSTODO: Modular renderer
	// BSTODO: Move renderer logic into module and store all data inside this module

	class FResourceManager final : public TSingleton<FResourceManager>
	{
	public:
		FResourceManager();

		virtual void Init() override;
		virtual void Dispose() override;

		static TUniquePtr<IVertexArray> CreateVertexArray();
		static TUniquePtr<IVertexBuffer> CreateVertexBuffer(const TVector<FVertex>& Vertices);
		static TUniquePtr<IIndexBuffer> CreateIndexBuffer(const TVector<uint32_t>& Indices);
		static TUniquePtr<IUniformBuffer> CreateUniformBuffer(const uint32_t Size, const uint32_t Binding);
		static TUniquePtr<IFrameBuffer> CreateFrameBuffer(const FFrameBufferSettings& Settings);
		static TUniquePtr<IShader> CreateShader(FStringView Name,
			FStringView VertexShaderSrc,
			FStringView FragmentShaderSrc);
		static TUniquePtr<IShader> CreateShaderFromFile(FStringView Name,
			FStringView VertexShaderPath,
			FStringView FragmentShaderPath);
		static TUniquePtr<ITexture> CreateTexture(FStringView Path, const bool bFlipped);
		static TUniquePtr<ITexture> CreateTextureFromMemory(const void* const Data, const uint32_t DataSize);
	};
}
