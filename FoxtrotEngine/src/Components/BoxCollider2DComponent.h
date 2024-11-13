#pragma once
#include "Components/Collider2DComponent.h"

class BoxCollider2DComponent :
    public Collider2DComponent
{
public:
	virtual std::string GetName() const override
	{
		return "BoxCollider2DComponent";
	}
	const	FTVector2	GetScale() const;
			void		SetScale(FTVector2 scale);

public:
	virtual void Initialize(FTCore* coreInstance) override;

public:
	BoxCollider2DComponent(
		Actor* owner,
		int drawOrder = DEFAULT_DRAWORDER,
		int updateOrder = DEFAULT_UPDATEORDER
	);

private:
	FTVector2 mScale;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
			void EditorUIUpdate() override;
			void RenderDebugGeometries(
				ImDrawList* imDrawList,
				FTVector2 screenCenter
			) override;

private:
	void UpdateScale();
#endif // FOXTROT_EDITOR
};

