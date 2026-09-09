#pragma once
#include <type_traits>

namespace Common
{
	namespace FTDS
	{
		/// <summary>
		/// Insertion sort for an array, only works for arrays with numerical values.
		/// </summary>
		/// <typeparam name="NUM : ">Type of the numerical value</typeparam>
		/// <param name="array : ">An array</param>
		/// <param name="count : ">The portion of an array to be sorted starting from array[0]. Insert the lenght of an array if fully sorted.</param>
		template <typename NUM, typename std::enable_if<std::is_arithmetic<NUM>::value>::type* = nullptr>
		/// @brief Reorders numeric elements in ascending order by inserting each element into the preceding sorted range.
		/// @param array Contiguous array of elements.
		/// @param count Number of entries to process.
		void InsertionSort(NUM* array, size_t count)
		{
			size_t i = 1;
			while (i < count)
			{
				NUM	   current = array[i];
				size_t j	   = i - 1;
				while (0 <= j && current < array[j])
				{
					array[j + 1] = array[j];
					--j;
				}
				array[j + 1] = current;
				++i;
			}
		}
	} // namespace FTDS
} // namespace Common
