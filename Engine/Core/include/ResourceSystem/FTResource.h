// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once

#include "ResourceSystem/IResource.h"
#include "Debugging/DebugMemAlloc.h"

namespace Common
{
	struct FTResourceDef { const char* FileName; const char* Path; int RefCount = 0; };

	class FTResource :
		public IResource
	{
	public:
		void SaveProperties(std::ofstream& ofs) override
		{
			if (mMetaData)
				mMetaData->SaveProperties(ofs);
		}

		void LoadProperties(std::ifstream& ifs) override
		{
			if (mMetaData)
				mMetaData->LoadProperties(ifs);
		}

		Common::FTDS::String* GetFileName() override
		{
			return mMetaData ? mMetaData->FileName : nullptr;
		}

		Common::FTDS::String* GetRelativePath() override
		{
			return mMetaData ? mMetaData->Path : nullptr;
		}

		void SetFileName(Common::FTDS::String& val) override
		{
			if (mMetaData && mMetaData->FileName)
				mMetaData->FileName->Assign(val);
		}

		void SetRelativePath(Common::FTDS::String& val) override
		{
			if (mMetaData && mMetaData->Path)
				mMetaData->Path->Assign(val);
		}

#ifdef FOXTROT_EDITOR
		virtual void UpdateUI() {}

		bool IsReferenced() const
		{
			return mMetaData && 0 < mMetaData->RefCount;
		}

		virtual void AddRefCount()
		{
			if (mMetaData)
				++mMetaData->RefCount;
		}

		virtual void SubtractRefCount()
		{
			if (mMetaData)
				--mMetaData->RefCount;
		}
#endif

	public:
		FTResource()
			: mMetaData(DBG_NEW ResourceData())
		{
			mMetaData->FileName = DBG_NEW Common::FTDS::String;
			mMetaData->Path		= DBG_NEW Common::FTDS::String;
#ifdef FOXTROT_EDITOR
			mMetaData->RefCount = 0;
#endif
		}

		explicit FTResource(FTResourceDef& resDef)
			: mMetaData(DBG_NEW ResourceData())
		{
			mMetaData->FileName = new Common::FTDS::String(resDef.FileName);
			mMetaData->Path = new Common::FTDS::String(resDef.Path);
#ifdef FOXTROT_EDITOR
			mMetaData->RefCount = resDef.RefCount;
#endif
		}

		~FTResource() override
		{
			if (mMetaData)
			{
				delete mMetaData->FileName;
				delete mMetaData->Path;
				delete mMetaData;
			}
		}

	protected:
		ResourceData* mMetaData;
	};
} // namespace Common
