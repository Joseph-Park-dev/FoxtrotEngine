#pragma once
#include "Components/MeshRenderer.h"

#include "spine/spine.h"

class SpineAnimator :
	public MeshRenderer
{
public:
	void		 Initialize(FTCore* coreInst) override;
	virtual void Update(float deltaTime) override;
	void		 Render(FoxtrotRenderer* renderer) override;

public:
	SpineAnimator(Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);
	~SpineAnimator() override;
	virtual void CloneTo(Actor* actor) override;

private:
	spine::Skeleton* mSkeleton;
	spine::AnimationState* mState;
	float				   mTimeScale;
	spine::Physics		   mUsePhysics;

private:
	void InitMeshData(FTMeshData& meshData, spine::RenderCommand* command);

#ifdef FOXTROT_EDITOR
	virtual void EditorUpdate(float deltaTime) override;

#endif // FOXTROT_EDITOR

};