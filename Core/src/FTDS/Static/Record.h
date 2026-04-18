#pragma once
#include "FTDS/Static/FTString.h"

namespace Core
{
	namespace FTDS
	{
		template <typename TYPE>
		class Record
		{
		public:
			bool Equal(String&& key) { return mKey.Equal(key.C_Str()); }
			bool Equal(const char* key) { return mKey.Equal(std::move(key)); }

		public:
			String& Key() { return mKey; }
			TYPE&	Value() { return mValue; }

		public:
			Record(Record* rec)
			{
				rec->mKey	= this->mKey;
				rec->mValue = this->mValue;
			}

			Record(String key, TYPE value)
			{
				mKey.Assign(key);
				mValue = value;
			}

		private:
			String mKey;
			TYPE   mValue;
		};
	} // namespace FTDS
} // namespace Core