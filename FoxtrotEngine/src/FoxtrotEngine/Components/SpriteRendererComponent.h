#pragma once
#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

class Actor;
class FTTexture;
class FoxtrotRenderer;
struct Mesh;

#define MAPKEY_SPRITE_MESH "BasicSquare"

class SpriteRendererComponent : public MeshRendererComponent
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
	virtual void SetTexture		()		override;
	void		 UpdateTexture	(FoxtrotRenderer* renderer, std::string fileName);

public:
			void Initialize		(FTCore* coreInstance);
	virtual void Update			(float deltaTime)			override;
	virtual void Render			(FoxtrotRenderer* renderer)	override;

public:
	virtual bool InitializeMesh	()		override;

public:
			SpriteRendererComponent(
				Actor* owner, 
				int drawOrder = DEFAULT_DRAWORDER, 
				int updateOrder = DEFAULT_UPDATEORDER
			 );
	virtual ~SpriteRendererComponent() override;

private:
	// These fields need to be loaded from .chunk file.
	int					mTexWidth;
	int					mTexHeight;
	int					mChannel;
	float				mScale;

#ifdef _DEBUG
//This section will be omitted from Release mode!
public:
	virtual void SaveProperties(nlohmann::ordered_json& out) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorUIUpdate() override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

protected:
	virtual void OnConfirmUpdate() override;
			void UpdateSprite();

private:
	void UpdateTexWidth();
	void UpdateTexHeight();
	void UpdateScale();
#endif // DEBUG
};