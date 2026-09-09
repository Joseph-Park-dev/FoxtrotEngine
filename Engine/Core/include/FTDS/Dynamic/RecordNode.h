#pragma once
#include "FTDS/Static/Record.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class RecordNode : public FTDS::Record<TYPE>
		{
		public:
			/// @brief Returns the link used by this record node.
			/// @return Borrowed access to the link.
			/// @note Changes through the returned reference affect this object's stored state.
			RecordNode*& GetLink() { return mLink; }
			/// @brief Updates the link used by subsequent operations.
			/// @param next Replacement link.
			void		 SetLink(RecordNode* next) { mLink = next; }

		public:
			/// @brief Initializes a keyed record with no successor.
			/// @param key Lookup key identifying the stored entry.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the Record<TYPE> base or delegates to its constructor.
			RecordNode(Common::FTDS::String key, TYPE val)
				: Record<TYPE>(key, val)
				, mLink(nullptr)
			{
			}

		private:
			RecordNode* mLink;
		};
	} // namespace FTDS
} // namespace Common
