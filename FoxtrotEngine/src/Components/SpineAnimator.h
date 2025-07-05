#pragma once
#include "Components/MeshRenderer.h"

#include "spine/spine.h"

class FTSpineAnimation;

class SpineAnimator :
	public MeshRenderer
{
public:
	virtual FTDS::String GetName() const override { return "SpineAnimator"; }

public:
	void		 Initialize(FTCore* coreInst) override;
	virtual void Update(float deltaTime) override;
	void		 Render(FoxtrotRenderer* renderer) override;

public:
	SpineAnimator(Actor* owner, int updateOrder = DefaultVal::UPDATE_ORDER);
	~SpineAnimator() override;
	virtual void CloneTo(Actor* actor) override;

protected:
	virtual void UpdateMesh(
		Transform*		 transform,
		Camera*			 camInst,
		FoxtrotRenderer* renderer) override;

private:
	FTSpineAnimation* mAnim;
	spine::Atlas*	  mAtlas;
	float			  mTimeScale;
	spine::Physics	  mUsePhysics;

#ifdef FOXTROT_EDITOR
	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;
	virtual void EditorUIUpdate() override;

#endif // FOXTROT_EDITOR
};