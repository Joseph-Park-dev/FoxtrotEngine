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

class FTRectangle;

class BoxCollider2D :
	public Collider2D
{
public:
	virtual FTDS::String GetName() const override
	{
		return "BoxCollider2D";
	}
	const FTVector2 GetSize() const;
	void			SetSize(FTVector2 size);

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Setup() override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	BoxCollider2D(
		Actor* owner,
		int	   updateOrder = DefaultVal::UPDATE_ORDER);
	~BoxCollider2D() override;
	virtual void CloneTo(Actor* actor) override;

private:
	FTVector2	 mSize;
	FTRectangle* mDebugRect;

private:
	void UpdateDebugShape(Camera* camInst) override;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime) override;
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

public:
	void EditorUIUpdate() override;

private:
	void UpdateScale();
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* COLLIDER_SCALE = "Scale";
}