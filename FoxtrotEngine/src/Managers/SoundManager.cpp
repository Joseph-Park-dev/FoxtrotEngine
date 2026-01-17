#include "SoundManager.h"

#include "Static/CircularQueue.h"
#include <Managers/ResourceManager.h>
#include "ResourceSystem/Sound/Sound.h"

#ifdef FOXTROT_EDITOR
	#include <EditorUtils.h>
	#include <EditorResourceManager.h>
#endif

constexpr const size_t MAX_PENDING_AUDIO = 20;

void SoundManager::Initialize()
{
	mSoloud = DBG_NEW SoLoud::Soloud;
	mSoloud->init();
}

void SoundManager::Play(size_t index, bool isRepeated)
{
	if (!isRepeated)
		mPending->Enqueue(index);
	else
		mRepeated->PushBack(index);
}

void SoundManager::LoadSoundFromChunk(FTDS::String& key)
{
	Sound* sound = ResourceManager::GetInstance()->GetLoadedSound(key);
	mLoaded->PushBack(sound);
}

void SoundManager::Update()
{
	while (!mPending->IsEmpty())
	{
		size_t idx = mPending->Peek();
		if (mLoaded->At(idx))
		{
			SoLoud::Wav* wav = mLoaded->At(idx)->GetWav();
			mSoloud->play(*wav);
		}
		mPending->Dequeue();
	}

	for (size_t i = 0; i < mRepeated->GetSize(); ++i)
	{
		size_t idx	 = mRepeated->At(i);
		Sound* sound = mLoaded->At(idx);

		if (!sound->GetIsPlaying())
			mSoloud->play(*sound->GetWav());
	}
}

SoundManager::SoundManager()
	: mLoaded(DBG_NEW FTDS::DynamicArray<Sound*>)
	, mPending(DBG_NEW FTDS::LinkedQueue<size_t>)
	, mRepeated(DBG_NEW FTDS::DynamicArray<size_t>)
{
	mLoaded->Reserve(5);
	mRepeated->Reserve(5);
}

SoundManager::~SoundManager()
{
	mSoloud->deinit();
	delete mSoloud;

	delete mLoaded;
	delete mPending;
	delete mRepeated;
}

void SoundManager::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SoundManager::SOUND_MANAGER);

	// Save the loaded sound titles.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SoundManager::LOADED);
	size_t idx = 0;
	if (!mLoaded->IsEmpty())
	{
		mLoaded->IterateArray([&](Sound* sound) {
			FileIOHelper::SaveString(ofs, std::to_string(idx).c_str(), sound->GetFileName().C_Str());
			++idx;
		});
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SoundManager::LOADED);

	// Save the indices for repeated sounds.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SoundManager::REPEATED);
	idx = 0;
	if (!mRepeated->IsEmpty())
	{
		mRepeated->IterateArray([&](size_t soundIdx) {
			FileIOHelper::SaveSize(ofs, std::to_string(idx).c_str(), soundIdx);
			++idx;
		});
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SoundManager::REPEATED);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SoundManager::SOUND_MANAGER);
}

void SoundManager::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SoundManager::SOUND_MANAGER);

	std::pair<size_t, FTDS::String> pack =
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SoundManager::REPEATED);
	mRepeated->Reserve(pack.first);

	for (size_t i = 0; i < pack.first; ++i)
	{
		size_t value = 0;
		FileIOHelper::LoadSize(ifs, value);
		mRepeated->PushBack(value);
	}

	pack =
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SoundManager::LOADED);
	mLoaded->Reserve(pack.first);

	for (size_t i = 0; i < pack.first; ++i)
	{
		FTDS::String value = ChunkKey::NullVal::NULL_OBJECT;
		FileIOHelper::LoadBasicString(ifs, value);
		LoadSoundFromChunk(value);
	}
}

#ifdef FOXTROT_EDITOR
void SoundManager::UpdateUI(bool* opened)
{
	if (!ImGui::Begin("TileMap Manager", opened))
	{
		ImGui::End();
	}
	else
	{
		ImGui::Text("Loaded Sounds");
		mLoaded->IterateArray([&](Sound* sound) {
			if (sound)
				ImGui::Text(sound->GetFileName().C_Str());
		});

		LoadSoundFromEditior();
		ImGui::End();
	}
}

void SoundManager::LoadSoundFromEditior()
{
	Sound* sound = nullptr;
	FTEditorUtils::DisplayResSelection("Load Sound", EditorResourceManager::GetInstance()->GetSounds(), sound);
	if (sound)
		if (sound->IsReferenced())
		{
			mLoaded->PushBack(sound);
			sound->AddRefCount();
		}
}
#endif