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
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SOUND);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SOUND);
}

void Sound::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SOUND);
	FTResource::LoadProperties(ifs);
}

void Sound::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	assert(!FileName().IsEmpty());

	mWav = DBG_NEW SoLoud::Wav;
	mWav->load(RelativePath().C_Str());

	SetIsProcessed(true);
}

Sound::Sound()
	: FTResource()
	, mWav(nullptr)
	, mIsPlaying(false)
{
}

Sound::~Sound()
{
	delete mWav;
}