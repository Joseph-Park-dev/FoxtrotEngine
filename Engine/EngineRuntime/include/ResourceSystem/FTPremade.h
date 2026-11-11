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
#include "Plugin/CoreExports.h"
#include "ResourceSystem/FTResource.h"

namespace Common
{
	class IActor;
}

#ifdef FOXTROT_EDITOR
namespace Editor
{
	class EditorElement;
	class EditorScene;
	class ChunkLoader;

	using CREATE_EDITOR_ELEM		= EditorElement* (*)(Common::IActor*, int);
	using GET_EDITOR_SCENE			= EditorScene* (*)();
	using GET_EDITOR_CHUNK_LOADER	= ChunkLoader* (*)();
} // namespace Editor
#endif // FOXTROT_EDITOR

namespace Core
{
	class Actor;

	namespace ChunkKey
	{
		namespace FTPremade
		{
			constexpr const char* FT_PREMADE = "FTPremade";
		}
	} // namespace ChunkKey

	class FTPremade :
		public Common::FTResource
	{
	public:
		/// @brief Returns the .chunk data-pack key for this resource type.
		static const char* GetName()
		{
			return ChunkKey::FTPremade::FT_PREMADE;
		}

	public:
		/// @brief Returns the origin used by this ftpremade.
		/// @return Borrowed access to the origin.
		CORE_API Actor* GetOrigin();
		/// @brief Returns the is loaded used by this ftpremade.
		/// @return Current value of the is loaded flag.
		CORE_API bool GetIsLoaded();

	public:
		/// @brief Initializes a premade by loading the actor stored at resDef.Path.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param data Unused. Present so resource packs can construct this type.
		CORE_API FTPremade(Common::FTResourceDef& resDef, void* data = nullptr);
		/// @brief Releases the origin actor owned by this premade.
		CORE_API ~FTPremade() override;

	private:
		/// @brief Loads the actor template stored in the premade file.
		/// @param path Filesystem path of the .premade file.
		void Load(const char* path);
		/// @brief Drops the current origin and loads it again from this premade's path.
		void Reload();

		Actor* mOrigin;
		bool   mIsLoaded;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Writes actor into this premade's file and reloads the origin from that file.
		/// @param actor Actor whose properties and components are stored.
		void Overwrite(Common::IActor* actor);

		/// @brief Writes actor to the premade file described by resDef.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param actor Actor whose properties and components are stored.
		void Save(Common::FTResourceDef& resDef, Common::IActor* actor);

		/// @brief Draws controls that place or edit this premade in the editor.
		void UpdateUI() override;

	private:
		/// @brief Resolves editor callbacks exported by Editor.dll.
		void BindEditorFunctions();

		Editor::EditorElement*			mDummyForUI;
		Editor::CREATE_EDITOR_ELEM		mCreateEditorElemFunc;
		Editor::GET_EDITOR_SCENE		mGetEditorSceneFunc;
		Editor::GET_EDITOR_CHUNK_LOADER mGetEditorChunkLoaderFunc;
#endif // FOXTROT_EDITOR
	};

#ifdef FOXTROT_EDITOR
	extern "C"
	{
		/// @brief Creates or replaces a premade file and registers it with the resource manager.
		/// @param fileName File name, including the .premade extension, used as the lookup key.
		/// @param path Full path of the premade file.
		/// @param actor Actor whose properties and components are stored.
		/// @return Premade owned by the resource manager, or nullptr when the arguments are unusable.
		CORE_API FTPremade* CreatePremade(const char* fileName, const char* path, Common::IActor* actor);
	}
#endif // FOXTROT_EDITOR
} // namespace Core
