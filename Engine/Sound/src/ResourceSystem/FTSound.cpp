// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "ResourceSystem/FTSound.h"

#include "Plugin/Sound.h"
#include "Debugging/DebugFuncs.h"
#include "FileSystem/NullKeys.h"

#include "soloud.h"
#include "soloud_wav.h"

namespace Sound
{
	SoLoud::Wav* FTSound::GetWave() const
	{
		return mWave;
	}

	void FTSound::Play()
	{
		SoLoud::Soloud* engine = GetSoloud();
		if (!engine || !mWave || mWave->mSampleCount == 0)
			return;
		engine->play(*mWave);
	}

	FTSound::FTSound(Common::FTResourceDef& resDef, void* userData)
		: Common::FTResource(resDef)
		, mWave(DBG_NEW SoLoud::Wav)
	{
		(void)userData;
		if (!resDef.Path || Common::FTDS::StringEqual(resDef.Path, Common::ChunkKey::NullVal::NULL_OBJECT))
			return;

		const SoLoud::result loaded = mWave->load(resDef.Path);
		if (loaded != SoLoud::SO_NO_ERROR)
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to load sound");
	}

	FTSound::~FTSound()
	{
		delete mWave;
		mWave = nullptr;
	}
} // namespace Sound
