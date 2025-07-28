#pragma once
#include <Core/SingletonMacro.h>

#include <SoLoud/include/soloud.h>

#include <Static/Array.h>
#include <Dynamic/LinkedQueue.h>
#include <ResourceSystem/Sound/SoundInstance.h>

class SoundManager
{
	SINGLETON(SoundManager)

public:
	void Play(FTDS::String&& key, bool isRepeated);

public:
	void Initialize();
	void LateUpdate();

private:
	SoLoud::Soloud* mSoloud;

	FTDS::LinkedQueue<Sound*>* mPending;
	FTDS::DynamicArray<Sound*>* mRepeated;
	Sound*						mCurrentBGM;
};