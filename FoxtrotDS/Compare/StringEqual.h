#pragma once
#include <string>

namespace FTDS
{
	bool StringEqual(const char* left, const char* right) 
	{
		size_t length = strlen(left);
		// The size of two strings are not equal.
		if (length != strlen(right))
			return false;
		for (size_t i = 0; i < length; ++i)
			if (left[i] != right[i])
				return false;
		return true;
	}
}