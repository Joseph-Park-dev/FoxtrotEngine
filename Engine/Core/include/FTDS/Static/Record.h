#pragma once
#include "FTDS/Static/FTString.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class Record
		{
		public:
			/// @brief Compares stored characters or a record key with the supplied text.
			/// @param key Lookup key identifying the stored entry.
			/// @return True when the compared values match; otherwise false.
			bool Equal(String&& key) { return mKey.Equal(key.C_Str()); }
			/// @brief Compares stored characters or a record key with the supplied text.
			/// @param key Lookup key identifying the stored entry.
			/// @return True when the compared values match; otherwise false.
			bool Equal(const char* key) { return mKey.Equal(std::move(key)); }

		public:
			/// @brief Exposes the record's stored lookup key.
			/// @return Borrowed access to the key.
			/// @note Changes through the returned reference affect this object's stored state.
			String& Key() { return mKey; }
			/// @brief Exposes the record's stored payload.
			/// @return Borrowed access to the value.
			/// @note Changes through the returned reference affect this object's stored state.
			TYPE&	Value() { return mValue; }

		public:
			/// @brief Writes this record's fields into the supplied record.
			/// @param rec Record passed to this constructor.
			/// @note This overload assigns to rec and does not initialize this record from rec.
			Record(Record* rec)
			{
				rec->mKey	= this->mKey;
				rec->mValue = this->mValue;
			}

			/// @brief Initializes a lookup key and its associated value.
			/// @param key Lookup key identifying the stored entry.
			/// @param value Value to assign, insert, or process.
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
} // namespace Common
