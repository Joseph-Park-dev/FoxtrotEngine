#include "ResourceSystem/Animation/AnimationFrame.h"

#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Common;
	void AnimationFrame::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::SaveFloat(ofs, ChunkKey::AnimationFrame::DURATION, Duration);
	}

	void AnimationFrame::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::LoadFloat(ifs, Duration);
	}
} // namespace D3D11