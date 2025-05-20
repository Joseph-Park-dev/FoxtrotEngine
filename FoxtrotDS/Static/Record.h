#pragma once
#include "Compare/StringEqual.h"

namespace FTDS
{
	template <typename TYPE>
	class Record
	{
	public:
		bool Equal(FTDS::String& key) { return key.Equal(mKey.C_Str()); }

	public:
		FTDS::String& Key() { return mKey; }
		TYPE		  Value() { return mValue; }

	public:
		Record(FTDS::String key, TYPE value)
		{
			mKey   = key;
			mValue = value;
		}

	private:
		FTDS::String mKey;
		TYPE		 mValue;
	};
} // namespace FTDS