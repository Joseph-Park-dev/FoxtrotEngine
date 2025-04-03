#pragma once
#include "ResourceSystem/Mesh.h"

struct AnimationFrame : Mesh
{
	float Duration;

	void SaveProperties(std::ofstream& ofs);
	void LoadProperties(std::ifstream& ifs);
};

namespace ChunkKey
{
	namespace AnimationFrame
	{
		constexpr const char* DURATION = "Duration";
	}
}