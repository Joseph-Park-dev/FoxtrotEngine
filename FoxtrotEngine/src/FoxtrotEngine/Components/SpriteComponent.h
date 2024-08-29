#pragma once
#include "FoxtrotEngine/Components/Component.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

class FTTexture;
class FoxtrotRenderer;
struct Mesh;

#define SPRITE_FORMAT_SUPPORTED ".png, .jpeg"

class SpriteRendererComponent : public Component
{
public:
	int			 GetTexWidth()  const { return mTexWidth; }
	int			 GetTexHeight() const { return mTexHeight; }
	std::wstring GetName()		const override
	{
		return L"SpriteRendererComponent";
	}
	void		 SetTexWidth	(int texWidth)  { mTexWidth = texWidth; }
	void		 SetTexHeight	(int texHeight) { mTexHeight = texHeight; }
	void		 SetTexture		(FTTexture* texture);
	void		 UpdateTexture	(FoxtrotRenderer* renderer, std::string fileName);

public:
	virtual void Initialize		(FTCore* coreInstance)		override;
	virtual void Update			(float deltaTime)			override;
	virtual void Render			(FoxtrotRenderer* renderer)	override;

public:
			 SpriteRendererComponent(class Actor* owner, 
				 int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~SpriteRendererComponent() override;

private:
	int					mTexWidth;
	int					mTexHeight;
	int					mChannel;
	float				mScale;

	Mesh*				mMesh;
	FoxtrotRenderer*	mRenderer;

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