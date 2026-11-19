// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Plays an FTSound from this actor's sound map when triggered.
/// </summary>
#pragma once
#include "Component/IComponent.h"
#include "Plugin/SoundExports.h"
#include "Debugging/DebugMemAlloc.h"
#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"

namespace Sound
{
	class FTSound;

	namespace ChunkKey
	{
		namespace AudioPlayer
		{
			constexpr const char* NAME		   = "AudioPlayer";
			constexpr const char* SOUNDS	   = "Sounds";
			constexpr const char* TRIGGER_KEY  = "Trigger Key";
			constexpr const char* TRIGGERED	   = "Triggered";
		} // namespace AudioPlayer
	} // namespace ChunkKey

	class AudioPlayer : public Common::IComponent
	{
	public:
		const char*		GetName() override;
		Common::IActor* GetOwner() override;

		/// @brief Returns the sounds assigned to this player, keyed by file name.
		/// @return Borrowed map. The player does not own the FTSound objects.
		SOUND_API Common::FTDS::HashMap<FTSound*>* GetSounds();

		/// @brief Inserts a borrowed sound into this player's map.
		/// @param sound Sound owned by the Sound resource manager.
		SOUND_API void AddSound(FTSound* sound);

		/// @brief Plays the trigger-key sound, or the first assigned sound when the key is empty.
		SOUND_API void Play();
		/// @brief Plays the assigned sound immediately.
		SOUND_API void Trigger();
		/// @brief Selects a sound by file name and plays it.
		/// @param key File name stored in the sound map.
		SOUND_API void Trigger(const char* key);

	public:
		void Initialize() override;
		void Setup() override;
		void ProcessInput(InputSystem::IInputDevice* inputDevice) override;
		void Update(float deltaTime) override;
		void LateUpdate(float deltaTime) override;
		void Render(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;

	public:
		explicit AudioPlayer(Common::IActor* owner);
		~AudioPlayer() override;

		void CloneTo(Common::IActor* actor) override;

	public:
		void SaveProperties(std::ofstream& ofs) override;
		void LoadProperties(std::ifstream& ifs) override;

	private:
		Common::IActor*					   mOwner;
		Common::FTDS::HashMap<FTSound*>*   mSounds;
		Common::FTDS::String			   mTriggerKey;
		bool							   mTriggered;

	private:
		/// @brief Fills null map entries from the resource manager.
		void ResolveSounds();
		/// @brief Returns the sound that Play should start.
		FTSound* FindSound();

#ifdef FOXTROT_EDITOR
	public:
		void EditorUpdate(float deltaTime) override;
		void EditorRender(Graphics::IRenderer* renderer, Graphics::ICamera* camInst) override;
		void EditorUIUpdate() override;
#endif
	};
} // namespace Sound
