#pragma once

#ifdef BS_PROFILING_ON
#include "ProfileTimer.h"

#define BS_PROFILE_RANGE(Name) ::Bloodshot::FProfileTimer InternalProfileTimer(Name, false)
#define BS_PROFILE_FUNCTION() ::Bloodshot::FProfileTimer InternalProfileTimer(BS_FUNC_SIG, true)
#else
#define BS_PROFILE_RANGE(Name)
#define BS_PROFILE_FUNCTION()
#endif