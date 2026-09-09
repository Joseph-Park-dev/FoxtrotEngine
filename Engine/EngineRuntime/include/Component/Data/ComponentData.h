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

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// //////////////
		/// Chunk I/O ///
		/// //////////////
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs);
	};

	namespace ChunkKey
	{
		constexpr const char* UPDATE_ORDER = "UpdateOrder";
		constexpr const char* IS_ACTIVE	   = "Is Active";
	} // namespace ChunkKey
} // namespace Common
