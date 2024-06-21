#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

class FTTexture;
class Mesh;

#define SPRITE_FORMAT_SUPPORTED ".png, .jpeg"

class SpriteComponent :public Component
{
public:
	Mesh*		 GetMesh()		const { return mMesh; }
	MeshData&	 GetMeshData()		  { return mMeshData; }
	int			 GetTexWidth()  const { return mTexWidth; }
	Bounds*		 GetRect()		const { return rect; }
	int			 GetTexHeight() const { return mTexHeight; }
	std::wstring GetName()		const override
	{
		return L"SpriteComponent";
	}
	void		 SetTexWidth (int texWidth)  { mTexWidth = texWidth; }
	void		 SetTexHeight(int texHeight) { mTexHeight = texHeight; }
	void		 SetTexture  (FoxtrotRenderer* renderer, std::string fileName);
	void		 UpdateTexture(FoxtrotRenderer* renderer, std::string fileName);

public:
	virtual void Initialize	(FoxtrotRenderer* renderer);
	virtual void Update		(float deltaTime)		    override;
	virtual void Render		(FoxtrotRenderer* renderer) override;

public:
			 SpriteComponent(class Actor* owner, 
				 int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~SpriteComponent() override;

private:
	FoxtrotRenderer* mRenderer;
	Mesh*			 mMesh;
	MeshData		 mMeshData;
	Bounds*			 rect;
	int				 mTexWidth;
	int				 mTexHeight;
	int				 mChannel;
	float			 mScale;

	DirectX::SimpleMath::Vector3 mViewEyePos = { 0.0f, 0.0f, -10.0f };
	DirectX::SimpleMath::Vector3 mViewEyeDir = { 0.0f, 0.0f, 1.0f };
	DirectX::SimpleMath::Vector3 mViewUp = { 0.0f, 1.0f, 0.0f };
	float mProjFovAngleY = 70.0f;
	float mNearZ = 0.01f;
	float mFarZ = 100.0f;
	float mAspect;

#ifdef _DEBUG
//This section will be omitted from Release mode!
public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

	virtual void EditorUpdate(float deltaTime) override;
	void EditorUIUpdate() override;

private:
	void UpdateSprite(FoxtrotRenderer* renderer);
	void UpdateTexWidth();
	void UpdateTexHeight();
	void UpdateScale();
#endif // DEBUG
};