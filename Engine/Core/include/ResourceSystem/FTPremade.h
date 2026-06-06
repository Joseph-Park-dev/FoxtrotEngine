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
#include "IResource.h"

#include <iosfwd>

namespace Core
{
	class Actor;
	class FTCore;
	class Scene;

	class FTPremade :
		public Common::IResource
	{
	public:
		void SaveProperties(std::ofstream& ofs) override {};
		void LoadProperties(std::ifstream& ifs) override {};

	public:
		FTPremade(Common::ResourceData* resDef);
		~FTPremade();

	public:
		Actor* GetOrigin();
		bool   GetIsLoaded();

	private:
		Common::ResourceData* mData;

		// Member variable that holds the actual Actor Data.
		Actor* mOrigin;
		bool   mIsLoaded;

	private:
		void Load(const char* path);

		// #ifdef FOXTROT_EDITOR
		//	public:
		//		void Save(Common::FTResourceDef& resDef, Editor::EditorElement* ele);
		//
		//		virtual void UpdateUI() override;
		//
		//		/// @brief Creates FTPremade with existing EditorElement
		//		FTPremade(Common::FTResourceDef& resDef, Editor::EditorElement* ele);
		//
		//	private:
		//		Editor::EditorElement*			mDummyForUI;
		//		Editor::CREATE_EDITOR_ELEM		mCreateEditorElemFunc;
		//		Editor::GET_EDITOR_SCENE		mGetEditorSceneFunc;
		//		Editor::GET_EDITOR_CHUNK_LOADER mGetEditorChunkLoaderFunc;
		//
		// #endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace FTPremade
		{
			constexpr const char* FT_PREMADE = "FTPremade";
		}
	} // namespace ChunkKey
} // namespace Core