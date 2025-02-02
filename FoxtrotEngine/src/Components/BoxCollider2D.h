// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Collider shaped as a box, or a square or a rectangle.
/// </summary>

#pragma once
#include "Components/Collider2D.h"

#ifdef FOXTROT_EDITOR
#include "ResourceSystem/FTShape.h"
#endif // FOXTROT_EDITOR

class BoxCollider2D :
    public Collider2D
{
public:
	virtual std::string GetName() const override
	{
		return "BoxCollider2D";
	}
	const	FTVector2	GetScale() const;
			void		SetScale(FTVector2 scale);

public:
	virtual void Initialize(FTCore* coreInstance)	override;
	virtual void Setup()							override;

	virtual void CloneTo(Actor* actor)				override;

public:
	 BoxCollider2D(
		 Actor* owner,
		 int updateOrder = DEFAULT_UPDATEORDER
	 );
	~BoxCollider2D() override;

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
			void ToggleDebugShape() override;
			void UpdateDebugShapes(
				FTVector3 pos, 
				FTVector3 rot, 
				FTVector3 scale, 
				Camera* cameraInst
			) override;

private:
	FTRectangle* mDebugRect;

private:
	void UpdateScale();
#endif // FOXTROT_EDITOR
};