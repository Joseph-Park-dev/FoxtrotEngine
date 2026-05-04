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
			RecordNode*& GetLink() { return mLink; }
			void		 SetLink(RecordNode* next) { mLink = next; }

		public:
			RecordNode(FTDS::String key, TYPE val)
				: Record<TYPE>(key, val)
				, mLink(nullptr)
			{
			}

		private:
			RecordNode* mLink;
		};
	} // namespace FTDS
} // namespace Common