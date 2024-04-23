#pragma once
#include "Component.h"
#include "TemplateFunctions.h"

class FTTexture;

class SpriteComponent :public Component
{
public:
	virtual void SetTexture();

public:
	int			 GetTexWidth() const { return mTextWidth; }
	Bounds*		 GetRect() const { return rect; }
	int			 GetTexHeight() const { return mTexHeight; }
	std::wstring GetName() const override
	{
		return L"SpriteComponent";
	}
	void	  SetTexWidth(int texWidth) { mTextWidth = texWidth; }
	void	  SetTexHeight(int texHeight) { mTexHeight = texHeight; }

	CLONE(SpriteComponent);

protected:
	FTTexture* GetTexture() { return mTexture; }

public:
	virtual void Update(float deltaTime) override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	SpriteComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~SpriteComponent() override;

private:
	FTTexture*	mTexture;
	Bounds*		rect;
	int			mTextWidth;
	int			mTexHeight;

#ifdef _DEBUG
//This section will be omitted from Release mode!
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

	void BlitToGameview	 (Bounds* blitArea, Vector2 scale);
	void BlitToGameviewEx(Bounds* blitArea, Vector2 scale);
#endif // DEBUG
};