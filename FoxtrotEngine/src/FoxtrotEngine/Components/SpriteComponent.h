#pragma once
#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

class FTTexture;
class Mesh;

#define SPRITE_FORMAT_SUPPORTED ".png, .jpeg"

class SpriteComponent : public MeshRendererComponent
{
public:
	int			 GetTexWidth()  const { return mTexWidth; }
	int			 GetTexHeight() const { return mTexHeight; }
	std::wstring GetName()		const override
	{
		return L"SpriteComponent";
	}
	void		 SetTexWidth	(int texWidth)  { mTexWidth = texWidth; }
	void		 SetTexHeight	(int texHeight) { mTexHeight = texHeight; }
	void		 SetTexture		(FoxtrotRenderer* renderer, std::string fileName);
	void		 UpdateTexture	(FoxtrotRenderer* renderer, std::string fileName);

public:
	virtual void Initialize		(FTCore* coreInstance)	override;
	virtual void Update			(float deltaTime)		override;

public:
			 SpriteComponent(class Actor* owner, 
				 int drawOrder = DEFAULT_DRAWORDER, int updateOrder = DEFAULT_UPDATEORDER);
	virtual ~SpriteComponent() override;

private:
	int				 mTexWidth;
	int				 mTexHeight;
	int				 mChannel;
	float			 mScale;

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