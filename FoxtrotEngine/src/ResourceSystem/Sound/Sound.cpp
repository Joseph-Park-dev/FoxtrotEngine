#include "Sound.h"

#include <assert.h>

SoLoud::Wav* Sound::GetWav() const
{
	return mWav;
}

bool Sound::GetIsPlaying() const
{
	return mIsPlaying;
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