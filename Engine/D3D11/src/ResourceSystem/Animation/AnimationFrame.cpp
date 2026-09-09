#include "ResourceSystem/Animation/AnimationFrame.h"

#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Common;
	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void AnimationFrame::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::SaveFloat(ofs, ChunkKey::AnimationFrame::DURATION, Duration);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void AnimationFrame::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::LoadFloat(ifs, Duration);
	}
} // namespace D3D11
