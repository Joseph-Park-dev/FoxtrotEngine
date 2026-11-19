// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// One loaded sound. The SoLoud wave is the sound object this resource holds.
/// </summary>
#pragma once
#include "ResourceSystem/FTResource.h"
#include "Plugin/SoundExports.h"

namespace SoLoud
{
	class Wav;
}

namespace Sound
{
	class FTSound : public Common::FTResource
	{
	public:
		/// @brief Returns the .chunk data-pack key for this resource type.
		static const char* GetName()
		{
			return "FTSound";
		}

	public:
		/// @brief Returns the SoLoud wave held by this resource.
		/// @return Borrowed sound object. Null when allocation failed.
		SOUND_API SoLoud::Wav* GetWave() const;

		/// @brief Plays this sound on the plugin's SoLoud engine.
		SOUND_API void Play();

	public:
		/// @brief Loads a sound file into a SoLoud wave.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param userData Unused. Present so resource factories can construct this type.
		FTSound(Common::FTResourceDef& resDef, void* userData);
		~FTSound() override;

	private:
		SoLoud::Wav* mWave;
	};
} // namespace Sound
