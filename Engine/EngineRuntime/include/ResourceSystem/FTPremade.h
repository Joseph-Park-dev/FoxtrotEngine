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
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs) override {};
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs) override {};

	public:
		/// @brief Initializes metadata for a reusable actor template.
		/// @param resDef Resource definition containing the filename and source path.
		CORE_API FTPremade(Common::ResourceData* resDef);
		/// @brief Releases the resources managed by this instance during destruction.
		CORE_API ~FTPremade();

	public:
		/// @brief Returns the origin used by this ftpremade.
		/// @return Borrowed access to the origin.
		CORE_API Actor* GetOrigin();
		/// @brief Returns the is loaded used by this ftpremade.
		/// @return Current value of the is loaded flag.
		CORE_API bool   GetIsLoaded();

	private:
		Common::ResourceData* mData;

		// Member variable that holds the actual Actor Data.
		Actor* mOrigin;
		bool   mIsLoaded;

	private:
		/// @brief Loads and initializes the content identified by the supplied path or descriptor.
		/// @param path Filesystem path of the resource or project.
		CORE_API void Load(const char* path);

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
