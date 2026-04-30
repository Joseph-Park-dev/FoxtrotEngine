// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pre-defined Actors that is saved as a file.
/// This can be instantiated during the game loop through the function
/// "Instantiate".
/// Similar role as "Prefab"s in Unity game engine.
/// </summary>

#pragma once
#include "ResourceSystem/CoreResource.h"

#include <iosfwd>

#ifdef FOXTROT_EDITOR
	#include <functional>

	#include "EditorElement.h"

namespace Editor
{
	class EditorScene;
	class EditorElement;

	typedef EditorElement* (*CREATE_EDITOR_ELEM)(Core::Actor*, int);
} // namespace Editor
#endif // FOXTROT_EDITOR

namespace Core
{
	class Actor;
	class FTCore;
	class Scene;

	class FTPremade :
		public CoreResource
	{
	public:
		static inline Core::ResType Type = Core::ResType::PREMADE;

	public:
		void SaveProperties(std::ofstream& ofs) override {};
		void LoadProperties(std::ifstream& ifs) override {};

	public:
		FTPremade(FTResourceDef& resDef);
		~FTPremade();

	public:
		Actor* GetOrigin();
		bool   GetIsLoaded();

	private:
		// Member variable that holds the actual Actor Data.
		Actor* mOrigin;
		bool   mIsLoaded;

	private:
		void Load(const char* path);

#ifdef FOXTROT_EDITOR
	public:
		void Save(FTResourceDef& resDef, Editor::EditorElement* ele);

		virtual void UpdateUI() override;

		/// @brief Creates FTPremade with existing EditorElement
		FTPremade(FTResourceDef& resDef, Editor::EditorElement* ele);

	private:
		Editor::EditorElement* mDummyForUI;
		Editor::CREATE_EDITOR_ELEM mCreateEditorElemFunc;

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace FTPremade
		{
			constexpr const char* FT_PREMADE = "FTPremade";
		}
	} // namespace ChunkKey
} // namespace Core