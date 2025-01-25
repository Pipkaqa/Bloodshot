#pragma once

// BSTODO: Complete support different compilers and OS

extern int main(int Argc, char** Argv);

#ifdef _WIN64
#define BS_WIN64
#define BS_WINDOWS
#elif defined(_WIN32)
#define BS_WIN32
#define BS_WINDOWS
#elif defined(__APPLE__)
#define BS_APPLE
#elif defined(__linux__)
#define BS_LINUX
#endif

#ifdef BS_WINDOWS
#define NOMINMAX
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#undef GetObject
#endif

#include <stdlib.h>

#ifdef _MSC_VER
#define BS_MSVC
#elif defined(__GNUC__)
#define BS_GNU
#elif defined(clang)
#define BS_CLANG
#else
#error Unsupported compiler
#endif

#ifdef BS_MSVC
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD __attribute__((warn_unused_result))
#endif

#ifdef BS_MSVC
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __attribute__((always_inline))
#endif

#ifdef BS_MSVC
#define FALLTHROUGH [[fallthrough]]
#else
#define FALLTHROUGH __attribute__ ((fallthrough))
#endif

#ifdef BS_MSVC
#define BS_DEBUG_BREAK() __debugbreak()
#else
#include <signal.h>
#ifdef SIGTRAP
#define BS_DEBUG_BREAK() raise(SIGTRAP)
#else
#define BS_DEBUG_BREAK() raise(SIGABRT)
#endif
#endif

#define BS_TERMINATE() std::terminate()

#ifndef NDEBUG
#define BS_DEBUG
#endif

#ifdef BS_MSVC
#define BS_FUNC_SIG __FUNCSIG__
#else
#define BS_FUNC_SIG __PRETTY_FUNCTION__
#endif

#define THIRD_PARTY_INCLUDES_START
#define THIRD_PARTY_INCLUDES_END
