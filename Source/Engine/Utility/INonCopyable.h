#pragma once

namespace Bloodshot
{
	class INonCopyable
	{
	public:
		INonCopyable() {}
		INonCopyable(const INonCopyable& other) = delete;

		INonCopyable& operator=(const INonCopyable& other) = delete;
	};
}
