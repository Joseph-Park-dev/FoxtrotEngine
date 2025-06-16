#pragma once
#include "ResourceSystem/FTShape.h"

class FTRectangle : public FTShape
{
public:
	// Don't forget to DebugShape::AddShape() after this!
	virtual void Initialize(FoxtrotRenderer* renderer) override;

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