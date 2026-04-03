#pragma once
#include "ResourceSystem/Mesh/Mesh.h"

namespace D3D11
{
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
	} // namespace ChunkKey
} // namespace D3D11