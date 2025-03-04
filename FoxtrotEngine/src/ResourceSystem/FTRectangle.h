#pragma once
#include "ResourceSystem/FTShape.h"

class FTRectangle : public FTShape
{
public:
	FTRectArea* GetRectArea();

public:
	virtual void Initialize(FoxtrotRenderer* renderer) override;
	virtual void Update() override;
	void		 Update(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* camInst);

protected:
	virtual void UpdateConstantBufferModel() override;

public:
	FTRectangle();
	~FTRectangle() override;

private:
	FTRectArea* mRectArea;
};