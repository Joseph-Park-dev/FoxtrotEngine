#pragma once
#include "Compare/StringEqual.h"

namespace FTDS
{
	template <typename TYPE>
	class Record
	{
	public:
		bool Equal(FTDS::String&& key) { return mKey.Equal(key.C_Str()); }
		bool Equal(const char* key) { return mKey.Equal(std::move(key)); }

	public:
		FTDS::String& Key() { return mKey; }
		TYPE&		  Value() { return mValue; }

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