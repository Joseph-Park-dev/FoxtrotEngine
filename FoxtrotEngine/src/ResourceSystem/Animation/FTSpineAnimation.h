#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include <spine/spine.h>

#include <Dynamic/DynamicArray.h>

class FTSpineAnimation :
	public FTAnimation
{
public:
	void InitializeSpinAnim(ComPtr<ID3D11Device>& device);

	virtual void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		FTMaterial*					 mat);

public:
	spine::Skeleton*	   GetSkeleton() const;
	spine::AnimationState* GetAnimState() const;
	float				   GetTimeScale();

	void SetTimeScale(float val);
	void SetAnimation(int idx, bool loop);
	void ToggleSkin(size_t idx);

	void SetJSON(FTJSON* json);
	void SetAtlasTxt(FTText* txt);

	spine::Vector<spine::Animation*>& LoadedClips();
	FTDS::DynamicArray<Mesh*>*		  Meshes() = delete;

public:
	void Update(float deltaTime, spine::Physics physics);
	void Render(
		FoxtrotRenderer* renderer,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat) override;

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

private:
	FTJSON*							 mJSON;
	FTText*							 mAtlasTxt;
	float							 mTimeScale;
	spine::Vector<spine::Animation*> mLoadedClips;
	spine::Vector<spine::Skin*>		 mSkins;
	unsigned char					 mSkinCombination;
	int								 mCurrAnimIdx;

	spine::Atlas*			   mAtlas;
	spine::SkeletonData*	   mSkeletonData;
	spine::Skeleton*		   mSkeleton;
	spine::AnimationStateData* mStateData;
	spine::AnimationState*	   mState;

	FTDS::DynamicArray<SpineMesh*>* mMeshes;

private:
	void InitializeMeshes(
		ComPtr<ID3D11Device>&			 device,
		int								 order,
		void*							 attachment,
		SpineMesh::SPINE_ATTACHMENT_TYPE attachmentType);

	void UpdateBuffers(ComPtr<ID3D11DeviceContext>& context);
	void SetSkin();

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();

public:
	virtual void AddRefCount() override;
	virtual void SubtractRefCount() override;
#endif // FOXTROT_EDITOR
};

constexpr size_t MAX_SKIN_COUNT = 20;

namespace ChunkKey
{
	constexpr const char* JSON_KEY		   = "JSON Key";
	constexpr const char* ATLAS_KEY		   = "Atlas Key";
	constexpr const char* SKIN_COMBINATION = "Skin Combination";
} // namespace ChunkKey