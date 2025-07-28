// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// The individual audio resource unit that is loaded to the
/// ResourceManager.
/// </summary>

#pragma once
#include <ResourceSystem/FTResource.h>

#include <SoLoud/include/soloud.h>
#include <SoLoud/include/soloud_wav.h>

class Sound :
	public FTResource
{
public:
	SoLoud::Wav* GetWav() const;
	bool		 GetIsPlaying() const;

public:
	void Process(FTCore* coreInst) override;

public:
	Sound();
	~Sound() override;

private:
	SoLoud::Wav* mWav;
	bool		 mIsPlaying;
};

namespace ChunkKey
{
	constexpr const char* SOUND = "Sound";
}