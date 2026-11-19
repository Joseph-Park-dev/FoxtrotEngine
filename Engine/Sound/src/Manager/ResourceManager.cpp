// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Manager/ResourceManager.h"

#include <fstream>

#include "FileSystem/FileTypes.h"

namespace Sound
{
	void ResourceManager::LoadResourcesFromChunk(std::ifstream& ifs)
	{
		if (mSounds)
			mSounds->LoadResourcesFromChunk(ifs);
	}

	FTSound* ResourceManager::GetSound(Common::FTDS::String& key)
	{
		if (!mSounds)
			return nullptr;
		return mSounds->GetResource(key);
	}

	FTSound* ResourceManager::GetSound(const char* key)
	{
		if (!mSounds)
			return nullptr;
		return mSounds->GetResource(key);
	}

	Common::ResourcePack<FTSound>* ResourceManager::GetSounds()
	{
		return mSounds;
	}

	ResourceManager::ResourceManager()
		: mSounds(DBG_NEW Common::ResourcePack<FTSound>(16))
	{
	}

	ResourceManager::~ResourceManager()
	{
		delete mSounds;
		mSounds = nullptr;
	}

	ResourceManager* GetSoundResourceManager()
	{
		return ResourceManager::GetInstance();
	}

#ifdef FOXTROT_EDITOR
	void ResourceManager::LoadAllResourcesInAsset(Common::FTDS::String* dir)
	{
		if (!mSounds || !dir)
			return;
		mSounds->LoadAllResourcesInAsset(dir->C_Str(), Common::FileTypes::Sound::WAV);
	}

	void ResourceManager::EditorUIUpdate()
	{
		if (mSounds)
			mSounds->DisplayLoadedResources("Sounds");
	}
#endif
} // namespace Sound
