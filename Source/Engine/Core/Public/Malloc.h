#pragma once

#include "Platform.h"

NODISCARD void* Malloc(const size_t Size);

void Free(void* const Block, const size_t Size);
