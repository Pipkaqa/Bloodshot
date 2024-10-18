#pragma once

#include "FrameBuffer.h"
#include "IndexBuffer.h"
#include "MathLibrary.h"
#include "Shader.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "UniformBuffer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <list>
#include <unordered_map>

namespace Bloodshot
{
	struct FVertex;

	class FResourceManager final : public TSingleton<FResourceManager>
	{
	public:
		using FVertexArrayList = std::list<TUniquePtr<IVertexArray>>;
		using FVertexBufferList = std::list<TUniquePtr<IVertexBuffer>>;
		using FIndexBufferList = std::list<TUniquePtr<IIndexBuffer>>;
		using FUniformBufferList = std::list<TUniquePtr<IUniformBuffer>>;
		using FFrameBufferList = std::list<TUniquePtr<IFrameBuffer>>;
		using FNameShaderTable = std::unordered_map<std::string, TUniquePtr<IShader>>;

		FResourceManager();

		static inline const char* ResourcesPath = "Resources";
		static inline const char* SpritesExtension = ".png";

		FVertexArrayList VertexArrayList;
		FVertexBufferList VertexBufferList;
		FIndexBufferList IndexBufferList;
		FUniformBufferList UniformBufferList;
		FFrameBufferList FrameBufferList;
		FNameShaderTable ShadersTable;

		virtual void Init() override;
		virtual void Dispose() override;

		static IVertexArray* CreateVertexArray();
		static IVertexBuffer* CreateVertexBuffer(const std::vector<FVertex>& Vertices);
		static IIndexBuffer* CreateIndexBuffer(const std::vector<uint32_t>& Indices);
		static IUniformBuffer* CreateUniformBuffer(const uint32_t Size, const uint32_t Binding);
		static IFrameBuffer* CreateFrameBuffer(const FFrameBufferSettings& Settings);
		static IShader* LoadShader(std::string_view Name,
			std::string_view VertexShaderPath,
			std::string_view FragmentShaderPath,
			const bool bReloadIfExists = false);
		static IShader* CreateShader(std::string_view Name,
			std::string_view VertexShaderSrc,
			std::string_view FragmentShaderSrc,
			const bool bReloadIfExists = false);
	};
}
