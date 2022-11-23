#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
		DEFINE_PATTERN(CL_Send_CvarValue2, "56 57 E8 ? ? ? ? 8B F8 E8 ? ? ? ? 8B F0");
		DEFINE_PATTERN(clc_buffer, "68 ? ? ? ? 85 F6 74 ? 0F AE E8");
	}
}