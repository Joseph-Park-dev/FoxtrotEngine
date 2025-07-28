#include "SoundManager.h"

#include "Static/CircularQueue.h"
#include <Managers/ResourceManager.h>
#include <EditorResourceManager.h>

constexpr const size_t MAX_PENDING_AUDIO = 20;

void SoundManager::Initialize()
{
	mSoloud = DBG_NEW SoLoud::Soloud;
	mSoloud->init();
}

void SoundManager::Play(FTDS::String&& key, bool isRepeated)
{
	Sound* Sound = EditorResourceManager::GetInstance()->GetLoadedSound(key);

	if (!isRepeated)
		mPending->Enqueue(Sound);
	else
		mRepeated->PushBack(Sound);
}

void SoundManager::LateUpdate()
{
	while (!mPending->IsEmpty())
	{
		mSoloud->play(*mPending->Peek()->GetWav());
		mPending->Dequeue();
	}

	for (size_t i = 0; i < mRepeated->GetSize(); ++i)
	{
		if (!mRepeated->At(i)->GetIsPlaying())
			mSoloud->play(*mRepeated->At(i)->GetWav());
	}
}

SoundManager::SoundManager()
	: mPending(DBG_NEW FTDS::LinkedQueue<Sound*>)
	, mRepeated(DBG_NEW FTDS::DynamicArray<Sound*>)
{
}

SoundManager::~SoundManager()
{
	mSoloud->deinit();
	delete mSoloud;
	delete mPending;
	delete mRepeated;
}