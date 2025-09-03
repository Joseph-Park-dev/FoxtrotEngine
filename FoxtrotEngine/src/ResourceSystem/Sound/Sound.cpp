#include "Sound.h"

#include <assert.h>

#include <FileSystem/FileIOHelper.h>

SoLoud::Wav* Sound::GetWav() const
{
	return mWav;
}

bool Sound::GetIsPlaying() const
{
	return mIsPlaying;
}

void Sound::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Sound::SOUND);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::Sound::SOUND);
}

void Sound::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Sound::SOUND);
	FTResource::LoadProperties(ifs);
}

void Sound::Process()
{
	if (IsProcessed())
		return;

	assert(!GetFileName().IsEmpty());

	mWav = DBG_NEW SoLoud::Wav;
	mWav->load(GetRelativePath().C_Str());

	FTResource::Process();
}

Sound::Sound(FTResourceDef& resDef)
	: FTResource(resDef)
	, mWav(nullptr)
	, mIsPlaying(false)
{
}

Sound::~Sound()
{
	delete mWav;
}