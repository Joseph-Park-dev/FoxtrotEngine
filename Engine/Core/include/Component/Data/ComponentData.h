#pragma once
#include "Actor/IActor.h"
#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

namespace Core
{
	struct ComponentData
	{
		//////////////////
		/// Properties ///
		//////////////////
		const char*	  Name;
		Core::IActor* Owner;
		int			  UpdateOrder;
		bool		  IsActive;

		////////////////////////////
		/// Initialization Phase ///
		////////////////////////////
		bool IsInitialized;
		bool IsSetup;

		/////////////////
		/// Chunk I/O ///
		/////////////////
		virtual void SaveProperties(std::ofstream& ofs);
		virtual void LoadProperties(std::ifstream& ifs);

		///////////////////////////////
		/// Editor Member Functions ///
		///////////////////////////////
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst);
	};

	namespace ChunkKey
	{
		constexpr const char* COMPONENTS   = "Components";
		constexpr const char* UPDATE_ORDER = "UpdateOrder";
		constexpr const char* IS_ACTIVE	   = "Is Active";
	} // namespace ChunkKey
} // namespace Core