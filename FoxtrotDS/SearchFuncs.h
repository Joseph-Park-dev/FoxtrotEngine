#pragma once
#include <type_traits>

#include "Math/FTMath.h"

namespace FTDS
{
	template <typename NUM, typename std::enable_if<std::is_arithmetic<NUM>::value>::type* = nullptr>
	int BinarySearch(NUM* array, size_t count, NUM target)
	{
		size_t idxHigh = count - 1;
		size_t idxLow = 0;
		while (idxLow <= idxHigh)
		{
			size_t idxMid = static_cast<size_t>(std::floor(idxHigh + idxLow) / 2);

			if (array[idxMid] < target)
				idxLow = idxMid + 1;
			else if (target < array[idxMid])
				idxHigh = idxMid - 1;
			else
				return idxMid;
		}
		return -1;
	}
}