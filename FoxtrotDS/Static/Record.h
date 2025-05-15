#pragma once
#include "Compare/StringEqual.h"

namespace FTDS
{
	template <typename TYPE>
	class Record
	{
	public:
		bool Equal(const char* key) { return StringEqual(key, mKey); }

	public:
		const char* Key() { return mKey; }
		TYPE&		Value() { return mValue; }

	public:
		Record(const char* key, TYPE value)
		{
			mKey   = key;
			mValue = value;
		}

	private:
		const char* mKey;
		TYPE		mValue;
	};
} // namespace FTDS