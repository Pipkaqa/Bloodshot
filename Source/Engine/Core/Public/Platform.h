#pragma once

#include <stdlib.h>

#define FORCEINLINE __forceinline

#define NODISCARD [[nodiscard]]

#define FALLTHROUGH [[fallthrough]]

#define BS_DEBUG_BREAK __debugbreak()

#define BS_TERMINATE(Code) exit(Code)

#define THIRD_PARTY_INCLUDES_START
#define THIRD_PARTY_INCLUDES_END

#define BS_TESTED friend struct Test::FTestFramework

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define BS_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define BS_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define BS_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define BS_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define BS_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define BS_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define BS_FUNC_SIG __func__
#else
#define BS_FUNC_SIG "UNKNOWN BS_FUNC_SIG MACRO"
#endif

extern int main(int Argc, char** Argv);
