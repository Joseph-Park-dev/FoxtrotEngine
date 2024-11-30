#pragma once
#include "Components/MeshRendererComponent.h"

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/MeshData.h"

class Actor;
class FTTexture;
class FoxtrotRenderer;
struct Mesh;

class SpriteRendererComponent : public MeshRendererComponent
{
public:
	int			 GetTexWidth()  { return GetMeshGroup()->GetTexture()->GetTexWidth() * mTexScale.x; }
	int			 GetTexHeight() { return GetMeshGroup()->GetTexture()->GetTexHeight() * mTexScale.y; }

	std::string  GetName()		const override{
		return	"SpriteRendererComponent";
	}

public:
	virtual	void Initialize		(FTCore* coreInstance)		override;
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

private:
	// These fields need to be loaded from .chunk file.
	int		  mChannel;
	FTVector2 mTexScale;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	virtual void EditorUIUpdate() override;
	virtual void CloneTo(Actor* actor)	override;

protected:
	virtual void OnConfirmUpdate() override;
			void UpdateSprite();

private:
	void UpdateTexWidth();
	void UpdateTexHeight();
	void UpdateScale();
#endif // FOXTROT_EDITOR
};