#pragma once
#include <type_traits>

#include "FTMath.h"

namespace Common
{
	namespace FTDS
	{
		/// <summary>
		/// Binary Search for an array, working only for arrays with numerical values.
		/// Returns -1 if the target value is not found.
		/// </summary>
		/// <typeparam name="NUM : ">Type for the numerical value.</typeparam>
		/// <param name="array : ">An array.</param>
		/// <param name="count : ">The length of an array.</param>
		/// <param name="target : ">Value to search for.</param>
		/// <returns></returns>
		template <typename NUM, typename std::enable_if<std::is_arithmetic<NUM>::value>::type* = nullptr>
		/// @brief Searches an ascending numeric array by repeatedly narrowing the index interval.
		/// @param array Contiguous array of elements.
		/// @param count Number of entries to process.
		/// @param target Target object or value sought by the operation.
		/// @return Searches an ascending numeric array by repeatedly narrowing the index interval.
		/// @pre array must contain count elements sorted in ascending order.
		int BinarySearch(NUM* array, size_t count, NUM target)
		{
			size_t idxHigh = count - 1;
			size_t idxLow  = 0;
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
	} // namespace FTDS
} // namespace Common
