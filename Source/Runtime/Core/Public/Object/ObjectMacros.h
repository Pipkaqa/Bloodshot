#pragma once

#define BSSTRUCT(...)
#define BSCLASS(...)
#define BSFUNCTION(...)
#define BSPROPERTY(...)

#define GENERATED_BODY(...);

namespace Bloodshot
{
	// Options for BSSTRUCT
	enum
	{
		SerializedOnly
	};

	// Options for BSPROPERTY
	enum
	{
		Serialized,
		Replicated
	};
}
