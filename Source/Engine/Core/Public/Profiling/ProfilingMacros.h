#pragma once

#include "ProfileTimer.h"

#ifdef BS_PROFILING_ON
#define BS_PROFILE_RANGE(Name) ::Bloodshot::SProfileTimer InternalProfileTimer(Name, false)
#define BS_PROFILE_FUNCTION() ::Bloodshot::SProfileTimer InternalProfileTimer(BS_FUNC_SIG, true)
#else
#define BS_PROFILE_RANGE(Name)
#define BS_PROFILE_FUNCTION()
#endif