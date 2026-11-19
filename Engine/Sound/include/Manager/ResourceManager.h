// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Loads and owns FTSound resources for the Sound plugin.
/// </summary>
#pragma once
#include "Plugin/SoundExports.h"
#include "Utility/SingletonMacro.h"
#include "ResourceSystem/ResourcePack.h"
#include "ResourceSystem/FTSound.h"
#include "FTDS/Static/FTString.h"

namespace Sound
{
	class ResourceManager
	{
		SINGLETON(ResourceManager)

	public:
		/// @brief Restores sound entries from a .chunk data pack.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		void LoadResourcesFromChunk(std::ifstream& ifs);

	public:
		/// @brief Returns the sound stored under the supplied key.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed sound. Null when the key is missing.
		FTSound* GetSound(Common::FTDS::String& key);
		/// @brief Returns the sound stored under the supplied key.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed sound. Null when the key is missing.
		FTSound* GetSound(const char* key);

		/// @brief Returns the sound pack owned by this manager.
		/// @return Borrowed pack. The manager owns the sounds inside it.
		Common::ResourcePack<FTSound>* GetSounds();

	private:
		Common::ResourcePack<FTSound>* mSounds;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Loads every supported sound file under the asset directory.
		/// @param dir Asset directory to scan.
		void LoadAllResourcesInAsset(Common::FTDS::String* dir);
		/// @brief Draws the loaded sounds in the editor resource browser.
		void EditorUIUpdate();
#endif
	};

	/// @brief Returns the resource manager owned by Sound.dll.
	/// @return Borrowed manager. Call this from other modules instead of GetInstance().
	SOUND_API ResourceManager* GetSoundResourceManager();
} // namespace Sound
