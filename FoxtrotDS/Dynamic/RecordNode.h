#pragma once
#include "Record.h"

namespace FTDS
{
	template <typename TYPE>
	class RecordNode : public FTDS::Record<TYPE>
	{
	public:
		RecordNode* GetLink() { return mLink; }
		void		SetLink(RecordNode* next) { mLink = next; }

	public:
		RecordNode(const char* key, TYPE val)
			: Record<TYPE>(key, val)
			, mLink(nullptr)
		{
		}

	private:
		RecordNode* mLink;
	};
} // namespace FTDS