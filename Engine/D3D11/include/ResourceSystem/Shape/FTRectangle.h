#pragma once
#include "ResourceSystem/Shape/FTShape.h"

#include <fstream>

#include <Plugin/D3D11Exports.h>

namespace D3D11
{
	namespace ProcName
	{
		constexpr const char* CREATE_FT_RECTANGLE = "CreateFTRectangle";
	}

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

	extern "C" D3D11_API D3D11::FTRectangle* CreateFTRectangle(); 

	namespace ChunkKey
	{
		constexpr const char* FTRECTANGLE = "FTRectangle";
	}
} // namespace D3D11