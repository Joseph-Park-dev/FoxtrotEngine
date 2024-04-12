#pragma once
#include <SDL2/SDL.h>

#include "Component.h"
#include "TemplateFunctions.h"

class Texture;

class SpriteComponent :public Component
{
public:
	virtual void SetTexture(Texture* texture);

public:
	int			 GetTexWidth() const { return mTextWidth; }
	SDL_Rect*	 GetRect() const { return rect; }
	int			 GetTexHeight() const { return mTexHeight; }
	std::wstring GetName() const override
	{
		return L"SpriteComponent";
	}
	void	  SetTexWidth(int texWidth) { mTextWidth = texWidth; }
	void	  SetTexHeight(int texHeight) { mTexHeight = texHeight; }

	CLONE(SpriteComponent);

protected:
	Texture* GetTexture() { return ptTexture; }

public:
	virtual void Update(float deltaTime) override;
	virtual void Render(SDL_Renderer* renderer) override;

public:
	SpriteComponent(class Actor* owner, int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	~SpriteComponent();

private:
	Texture*	ptTexture;
	SDL_Rect*	rect;
	int			mTextWidth;
	int			mTexHeight;

	SDL_Surface* mGameviewSurf;

#ifdef _DEBUG
//This section will be omitted from Release mode!
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

	void BlitToGameview(SDL_Rect* blitArea, Vector2 scale);
	void BlitToGameviewEx(SDL_Rect* blitArea, Vector2 scale);
#endif // DEBUG
};