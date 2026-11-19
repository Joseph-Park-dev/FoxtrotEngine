// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Component/AudioPlayer.h"

#include <string>

#include "Actor/IActor.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/NullKeys.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/ResourceManager.h"
#include "Plugin/Sound.h"
#include "ResourceSystem/FTSound.h"

#ifdef FOXTROT_EDITOR
	#include "Plugin/ICommandHistory.h"
#endif

namespace Sound
{
	const char* AudioPlayer::GetName()
	{
		return ChunkKey::AudioPlayer::NAME;
	}

	Common::IActor* AudioPlayer::GetOwner()
	{
		return mOwner;
	}

	Common::FTDS::HashMap<FTSound*>* AudioPlayer::GetSounds()
	{
		return mSounds;
	}

	void AudioPlayer::AddSound(FTSound* sound)
	{
		if (!mSounds || !sound || !sound->GetFileName())
			return;

		Common::FTDS::String&				key		= *sound->GetFileName();
		Common::FTDS::Record<FTSound*>*		existing = mSounds->At(key);
		if (existing)
		{
#ifdef FOXTROT_EDITOR
			const bool wasEmpty = existing->Value() == nullptr;
#endif
			existing->Value() = sound;
#ifdef FOXTROT_EDITOR
			if (wasEmpty)
				sound->AddRefCount();
#endif
			return;
		}

		mSounds->Insert(key, sound);
#ifdef FOXTROT_EDITOR
		sound->AddRefCount();
#endif
	}

	void AudioPlayer::ResolveSounds()
	{
		ResourceManager* resources = ResourceManager::GetInstance();
		if (!resources || !mSounds)
			return;

		for (auto iter = mSounds->Begin(); iter != mSounds->End(); ++iter)
		{
			if (!*iter || (*iter)->Value())
				continue;
			if (FTSound* sound = resources->GetSound((*iter)->Key()))
				AddSound(sound);
		}
	}

	FTSound* AudioPlayer::FindSound()
	{
		if (!mSounds)
			return nullptr;

		const bool named = !mTriggerKey.IsEmpty() && !mTriggerKey.Equal(Common::ChunkKey::NullVal::NULL_OBJECT);
		if (named)
		{
			if (Common::FTDS::Record<FTSound*>* record = mSounds->At(mTriggerKey))
			{
				if (!record->Value())
				{
					if (ResourceManager* resources = ResourceManager::GetInstance())
						record->Value() = resources->GetSound(mTriggerKey);
				}
				if (record->Value())
					return record->Value();
			}
			else if (ResourceManager* resources = ResourceManager::GetInstance())
			{
				if (FTSound* sound = resources->GetSound(mTriggerKey))
				{
					AddSound(sound);
					return sound;
				}
			}
			return nullptr;
		}

		for (auto iter = mSounds->Begin(); iter != mSounds->End(); ++iter)
		{
			if (*iter && (*iter)->Value())
				return (*iter)->Value();
		}
		return nullptr;
	}

	void AudioPlayer::Play()
	{
		ResolveSounds();
		if (FTSound* sound = FindSound())
			sound->Play();
	}

	void AudioPlayer::Trigger()
	{
		Play();
	}

	void AudioPlayer::Trigger(const char* key)
	{
		if (key && key[0])
			mTriggerKey.Assign(key);
		Play();
	}

	void AudioPlayer::Initialize()
	{
		ResolveSounds();
	}

	void AudioPlayer::Setup()
	{
	}

	void AudioPlayer::ProcessInput(InputSystem::IInputDevice* inputDevice)
	{
		(void)inputDevice;
	}

	void AudioPlayer::Update(float deltaTime)
	{
		(void)deltaTime;
		if (!mTriggered)
			return;
		mTriggered = false;
		Play();
	}

	void AudioPlayer::LateUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void AudioPlayer::Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	AudioPlayer::AudioPlayer(Common::IActor* owner)
		: mOwner(owner)
		, mSounds(DBG_NEW Common::FTDS::HashMap<FTSound*>(16))
		, mTriggerKey()
		, mTriggered(false)
	{
	}

	AudioPlayer::~AudioPlayer()
	{
		delete mSounds;
		mSounds = nullptr;
		UnregisterOwnedComponent(this);
	}

	void AudioPlayer::CloneTo(Common::IActor* actor)
	{
		AudioPlayer* copy = DBG_NEW AudioPlayer(actor);
		copy->mTriggerKey.Assign(mTriggerKey);
		copy->mTriggered = mTriggered;
		if (mSounds)
		{
			for (auto iter = mSounds->Begin(); iter != mSounds->End(); ++iter)
			{
				if (*iter && (*iter)->Value())
					copy->AddSound((*iter)->Value());
			}
		}
		if (actor && actor->GetComponents())
			actor->GetComponents()->PushBack(copy);
		RegisterOwnedComponent(copy);
	}

	void AudioPlayer::SaveProperties(std::ofstream& ofs)
	{
		// The chunk buffer is written in reverse, so LoadProperties reads these last-to-first.
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::AudioPlayer::SOUNDS);
		size_t index = 0;
		if (mSounds)
		{
			for (auto iter = mSounds->Begin(); iter != mSounds->End(); ++iter)
			{
				if (!*iter)
					continue;
				Common::FileIOHelper::SaveString(ofs, std::to_string(index).c_str(), (*iter)->Key());
				++index;
			}
		}
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::AudioPlayer::SOUNDS);
		Common::FileIOHelper::SaveString(ofs, ChunkKey::AudioPlayer::TRIGGER_KEY, mTriggerKey);
		Common::FileIOHelper::SaveBool(ofs, ChunkKey::AudioPlayer::TRIGGERED, mTriggered);
	}

	void AudioPlayer::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::LoadBool(ifs, mTriggered);
		Common::FileIOHelper::LoadBasicString(ifs, mTriggerKey);

		std::pair<size_t, Common::FTDS::String> pack =
			Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::AudioPlayer::SOUNDS);
		ResourceManager* resources = ResourceManager::GetInstance();
		for (size_t i = 0; i < pack.first; ++i)
		{
			Common::FTDS::String key;
			Common::FileIOHelper::LoadBasicString(ifs, key);
			FTSound* sound = resources ? resources->GetSound(key) : nullptr;
			if (sound)
				AddSound(sound);
			else if (mSounds && !mSounds->At(key))
				mSounds->Insert(key, nullptr);
		}
		ResolveSounds();
	}

#ifdef FOXTROT_EDITOR
	void AudioPlayer::EditorUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	void AudioPlayer::EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst)
	{
		(void)renderer;
		(void)camInst;
	}

	void AudioPlayer::EditorUIUpdate()
	{
		Editor::ICommandHistory* history = Editor::GetCommandHistory();
		if (!history)
			return;

		history->UpdateBoolValue(ChunkKey::AudioPlayer::TRIGGERED, mTriggered);
		history->UpdateStringValue(ChunkKey::AudioPlayer::TRIGGER_KEY, mTriggerKey);

		if (mTriggerKey.IsEmpty() || mTriggerKey.Equal(Common::ChunkKey::NullVal::NULL_OBJECT))
			return;
		if (mSounds && mSounds->At(mTriggerKey))
			return;

		ResourceManager* resources = ResourceManager::GetInstance();
		if (!resources || !resources->GetSounds() || !resources->GetSounds()->GetResMap())
			return;
		Common::FTDS::Record<FTSound*>* record = resources->GetSounds()->GetResMap()->At(mTriggerKey);
		if (record && record->Value())
			AddSound(record->Value());
	}
#endif
} // namespace Sound
