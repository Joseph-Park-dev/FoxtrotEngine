#pragma once
#include <iosfwd>

namespace Editor
{
	class CommandHistory;
}

namespace Common
{
	class IActor;

	struct ComponentData
	{
		//////////////////
		/// Properties ///
		//////////////////
		const char*		Name;
		Common::IActor* Owner;
		int				UpdateOrder;
		bool			IsActive;

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
	};

	namespace ChunkKey
	{
		constexpr const char* UPDATE_ORDER = "UpdateOrder";
		constexpr const char* IS_ACTIVE	   = "Is Active";
	} // namespace ChunkKey
} // namespace Common