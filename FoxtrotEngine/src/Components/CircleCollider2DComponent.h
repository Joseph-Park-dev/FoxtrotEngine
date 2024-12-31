#pragma once
#include "Components/Collider2DComponent.h"

class CircleCollider2DComponent :
    public Collider2DComponent
{
public:
	virtual std::string GetName() const override
	{
		return "BoxCollider2DComponent";
	}
	const	float		GetRadius() const;
			void		SetRadius(float radius);

public:
	virtual void Initialize(FTCore* coreInstance) override;

public:
	CircleCollider2DComponent(
		Actor* owner,
		int drawOrder = DEFAULT_DRAWORDER,
		int updateOrder = DEFAULT_UPDATEORDER
	);

private:
	FTVector2	mCenter;
	float		mRadius;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime)			 override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	void EditorUIUpdate() override;

private:
	void UpdateScale();
#endif // FOXTROT_EDITOR
};

