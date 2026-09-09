#pragma once
#include "ResourceSystem/Mesh/Mesh.h"

namespace D3D11
{
	struct AnimationFrame : Mesh
	{
		float Duration;

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs);
	};

	namespace ChunkKey
	{
		namespace AnimationFrame
		{
			constexpr const char* DURATION = "Duration";
		}
	} // namespace ChunkKey
} // namespace D3D11
