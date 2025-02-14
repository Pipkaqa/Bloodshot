#pragma once

#define BSSTRUCT(...)
#define BSCLASS(...)
#define BSFUNCTION(...)
#define BSPROPERTY(...)

#define GENERATED_BODY(...) \
friend class ::Bloodshot::Private::FObjectCore; \
template<typename ClassType, typename FunctionType, IsObject ObjectType, typename... ArgTypes> \
friend struct ::Bloodshot::IFunctionCaller; \
private: static FClass* GetPrivateStaticClass(); \
public: inline static FClass* StaticClass() { return GetPrivateStaticClass(); }

namespace Bloodshot
{
	// Options for BSSTRUCT
	namespace BS
	{
		enum
		{
			SerializedOnly
		};
	}

	// Options for BSPROPERTY
	namespace BP
	{
		enum
		{
			Serialized,
			Replicated
		};
	}
}
