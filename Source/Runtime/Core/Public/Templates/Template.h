#pragma once

#include "Templates/IsContiguousContainer.h"

namespace Bloodshot
{
	template<IsContiguousContainer T>
	constexpr auto GetData(T&& Container) -> decltype(Container.GetData())
	{
		return Container.GetData();
	}

	template <typename T, size_t Size>
	constexpr T* GetData(T(&Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	constexpr T* GetData(T(&& Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	constexpr const T* GetData(const T(&Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	constexpr const T* GetData(const T(&& Container)[Size])
	{
		return Container;
	}

	template <typename T>
	constexpr const T* GetData(std::initializer_list<T> InitList)
	{
		return InitList.begin();
	}

	template<IsContiguousContainer T>
	constexpr auto GetSize(T&& Container) -> decltype(Container.GetSize())
	{
		return Container.GetSize();
	}

	template <typename T, size_t Size>
	constexpr size_t GetSize(T(&Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	constexpr size_t GetSize(T(&& Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	constexpr size_t GetSize(const T(&Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	constexpr size_t GetSize(const T(&& Container)[Size])
	{
		return Size;
	}

	template <typename T>
	constexpr size_t GetSize(std::initializer_list<T> InitList)
	{
		return InitList.size();
	}
}
