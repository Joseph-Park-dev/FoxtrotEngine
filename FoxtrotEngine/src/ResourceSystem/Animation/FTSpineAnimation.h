#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include "spine/spine.h"

class FTSpineAnimation :
	public FTAnimation
{
public:
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	spine::Skeleton*	   GetSkeleton();
	spine::AnimationState* GetAnimState();
	float				   GetTimeScale();

	void SetTimeScale(float val);
	void SetMaterials(std::vector<FTDS::String>& matKeys, ComPtr<ID3D11Device>& device) override;

public:
	void Initialize(
		ComPtr<ID3D11Device>&	   device,
		spine::SkeletonData*	   skel,
		spine::AnimationStateData* stateData);
	virtual void Update(float deltaTime, spine::Physics physics);
	void		 Render(FoxtrotRenderer* renderer) override;

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

private:
	spine::Skeleton*		mSkeleton;
	spine::AnimationState*	mState;
	float					mTimeScale;
	std::vector<SpineMesh*> mMeshes;
	std::vector<Mesh*>&		Meshes() = delete;

private:
	void InitializeMeshes(
		ComPtr<ID3D11Device>&			 device,
		int								 order,
		void*							 attachment,
		SpineMesh::SPINE_ATTACHMENT_TYPE attachmentType);

	void InitializeConstantBuffers(ComPtr<ID3D11Device>& device) override;
	void UpdateBuffers(ComPtr<ID3D11DeviceContext>& context);
};
