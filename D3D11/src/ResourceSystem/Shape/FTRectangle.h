#pragma once
#include "ResourceSystem/Shape/FTShape.h"

namespace D3D11
{
	class FTRectangle : public FTShape
	{
	public:
		// Don't forget to DebugShape::AddShape() after this!
		virtual void Initialize(D3D11Renderer* renderer) override;

	public:
		FTRectangle();
		~FTRectangle() override;

	public:
		void SaveProperties(std::ofstream& ofs);
		void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI();
#endif
	};

	namespace ChunkKey
	{
		constexpr const char* FTRECTANGLE = "FTRectangle";
	}
} // namespace D3D11