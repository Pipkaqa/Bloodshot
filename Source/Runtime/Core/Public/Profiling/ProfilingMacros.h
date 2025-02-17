#pragma once

#ifdef BS_PROFILING_ON
#include "Profiling/ProfileTimer.h"

#define BS_PROFILE_RANGE(Name) ::Bloodshot::FProfileTimer __InternalProfileTimer(Name, false)
#define BS_PROFILE_FUNCTION() ::Bloodshot::FProfileTimer __InternalProfileTimer(BS_FUNC_SIG, true)
#else
#define BS_PROFILE_RANGE(Name)
#define BS_PROFILE_FUNCTION()
#endif
