#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include "spine/spine.h"

class FTSpineAnimation :
	public FTAnimation
{
public:
	void InitializeSpinAnim(
		ComPtr<ID3D11Device>& device,
		spine::SkeletonData*  skel);

	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context) override;

public:
	spine::Skeleton*	   GetSkeleton();
	spine::AnimationState* GetAnimState();
	float				   GetTimeScale();

	void SetJSONKey(FTDS::String& key);
	void SetAtlasKey(FTDS::String& key);
	void SetTimeScale(float val);
	void SetMaterials(std::vector<FTDS::String>& matKeys, ComPtr<ID3D11Device>& device) override;
	void SetAnimation(size_t idx, bool loop);
	void ToggleSkin(size_t idx);

	spine::Vector<spine::Animation*>& LoadedClips();

public:
	virtual void Update(float deltaTime, spine::Physics physics);
	void		 Render(FoxtrotRenderer* renderer) override;

public:
	FTSpineAnimation();
	~FTSpineAnimation() override;

private:
	FTDS::String					 mJSONKey;
	FTDS::String					 mAtlasKey;
	spine::Vector<spine::Animation*> mLoadedClips;
	spine::Vector<spine::Skin*>		 mSkins;
	unsigned char					 mSkinCombination;

	spine::SkeletonData*	   mSkeletonData;
	spine::AnimationStateData* mStateData;
	spine::Skeleton*		   mSkeleton;
	spine::Atlas*			   mAtlas;
	spine::AnimationState*	   mState;
	float					   mTimeScale;
	std::vector<SpineMesh*>	   mMeshes;
	std::vector<Mesh*>&		   Meshes() = delete;

private:
	void InitializeMeshes(
		ComPtr<ID3D11Device>&			 device,
		int								 order,
		void*							 attachment,
		SpineMesh::SPINE_ATTACHMENT_TYPE attachmentType);

	void InitializeConstantBuffers(ComPtr<ID3D11Device>& device) override;
	void UpdateBuffers(ComPtr<ID3D11DeviceContext>& context);
	void SetSkin();

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();
#endif // FOXTROT_EDITOR
};

constexpr size_t MAX_SKIN_COUNT = 20;

namespace ChunkKey
{
	constexpr const char* JSON_KEY		   = "JSON Key";
	constexpr const char* ATLAS_KEY		   = "Atlas Key";
	constexpr const char* SKIN_COMBINATION = "Skin Combination";
} // namespace ChunkKey