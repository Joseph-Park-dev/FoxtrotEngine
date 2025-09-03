#pragma once
#include "Components/MeshRenderer.h"

#include <spine/spine.h>

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
	virtual void CloneTo(Actor* actor) override;

private:
	float		   mTimeScale;
	spine::Physics mUsePhysics;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;
	virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace SpineAnimator
	{
		constexpr const char* LOADED_ANIM = "Loaded Anim";
	}
} // namespace ChunkKey