// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Collider shaped as a circle.
/// WIP in the current version.
/// </summary>

#pragma once
#include "Components/Collider2D.h"

class CircleCollider2D :
    public Collider2D
{
public:
	virtual std::string GetName() const override
	{
		return "BoxCollider2D";
	}
	const	float		GetRadius() const;
			void		SetRadius(float radius);

public:
	virtual void Initialize	(FTCore* coreInstance)	override;

	virtual void CloneTo	(Actor* actor)			override;

public:
	CircleCollider2D(
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

