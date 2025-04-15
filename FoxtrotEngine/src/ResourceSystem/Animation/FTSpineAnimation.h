#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include "spine/spine.h"

class FTSpineAnimation :
	public FTAnimation
{
public:
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

protected:
	virtual void InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes);

private:
	spine::Skeleton*	   mSkeleton;
	spine::AnimationState* mState;
	float				   mTimeScale;
};
