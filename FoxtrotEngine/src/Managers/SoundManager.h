#pragma once
#include <Core/Singleton.h>

#include <SoLoud/include/soloud.h>

#include <Static/Array.h>
#include <Dynamic/LinkedQueue.h>

class Sound;

class SoundManager
{
	SINGLETON(SoundManager)

public:
	void Play(size_t index, bool isRepeated);
	void LoadSoundFromChunk(FTDS::String& key);

public:
	void Initialize();
	void Update();

private:
	SoLoud::Soloud* mSoloud;

	FTDS::DynamicArray<Sound*>* mLoaded;
	FTDS::LinkedQueue<size_t>*	mPending;
	FTDS::DynamicArray<size_t>* mRepeated;
	Sound*						mCurrentBGM;

public:
	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);

private:
	void LoadSoundFromEditior();
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace SoundManager
	{
		constexpr const char* SOUND_MANAGER = "Sound Manager";
		constexpr const char* LOADED		= "Loaded Sounds";
		constexpr const char* REPEATED		= "Repeated Sounds";
	} // namespace SoundManager
} // namespace ChunkKey