#include "AnimationFrame.h"

void AnimationFrame::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::SaveFloat(ofs, ChunkKey::AnimationFrame::DURATION, Duration);
}

void AnimationFrame::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadFloat(ifs, Duration);
}
