#pragma once
#include "ResourceSystem/FTShaders/FTPixelShader.h"

struct StandardPSData;

class StandardPS :
	public FTPixelShader
{
public:
	StandardPS();

private:
	FTVector3 mAmbient;
	float	  mShininess;
	FTVector3 mDiffuse;
	FTVector3 mSpecular;

//public:
//	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
//	virtual UINT LoadProperties(std::ifstream& ifs) override;
};