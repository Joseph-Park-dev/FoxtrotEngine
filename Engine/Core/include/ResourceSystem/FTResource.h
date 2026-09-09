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
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs) override
		{
			if (mMetaData)
				mMetaData->SaveProperties(ofs);
		}

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs) override
		{
			if (mMetaData)
				mMetaData->LoadProperties(ifs);
		}

		/// @brief Returns the file name used by this ftresource.
		/// @return Borrowed access to the file name.
		Common::FTDS::String* GetFileName() override
		{
			return mMetaData ? mMetaData->FileName : nullptr;
		}

		/// @brief Returns the relative path used by this ftresource.
		/// @return Borrowed access to the relative path.
		Common::FTDS::String* GetRelativePath() override
		{
			return mMetaData ? mMetaData->Path : nullptr;
		}

		/// @brief Updates the file name used by subsequent operations.
		/// @param val Replacement file name.
		void SetFileName(Common::FTDS::String& val) override
		{
			if (mMetaData && mMetaData->FileName)
				mMetaData->FileName->Assign(val);
		}

		/// @brief Updates the relative path used by subsequent operations.
		/// @param val Replacement relative path.
		void SetRelativePath(Common::FTDS::String& val) override
		{
			if (mMetaData && mMetaData->Path)
				mMetaData->Path->Assign(val);
		}

#ifdef FOXTROT_EDITOR
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		virtual void UpdateUI() {}

		/// @brief Tests whether the resource metadata records any active references.
		/// @return True when the resource metadata records any active references; otherwise false.
		bool IsReferenced() const
		{
			return mMetaData && 0 < mMetaData->RefCount;
		}

		/// @brief Increments the resource metadata's reference count when metadata exists.
		virtual void AddRefCount()
		{
			if (mMetaData)
				++mMetaData->RefCount;
		}

		/// @brief Decrements the resource metadata's reference count when metadata exists.
		virtual void SubtractRefCount()
		{
			if (mMetaData)
				--mMetaData->RefCount;
		}
#endif

	public:
		/// @brief Initializes resource metadata, filename, and path storage.
		FTResource()
			: mMetaData(DBG_NEW ResourceData())
		{
			mMetaData->FileName = DBG_NEW Common::FTDS::String;
			mMetaData->Path		= DBG_NEW Common::FTDS::String;
#ifdef FOXTROT_EDITOR
			mMetaData->RefCount = 0;
#endif
		}

		/// @brief Initializes resource metadata, filename, and path storage.
		/// @param resDef Resource definition containing the filename and source path.
		explicit FTResource(FTResourceDef& resDef)
			: mMetaData(DBG_NEW ResourceData())
		{
			mMetaData->FileName = new Common::FTDS::String(resDef.FileName);
			mMetaData->Path = new Common::FTDS::String(resDef.Path);
#ifdef FOXTROT_EDITOR
			mMetaData->RefCount = resDef.RefCount;
#endif
		}

		/// @brief Releases the resources managed by this instance during destruction.
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
