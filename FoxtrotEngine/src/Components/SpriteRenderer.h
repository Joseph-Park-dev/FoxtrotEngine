// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base Component for rendering 2D sprites.
/// This uses FTTexture as an image source, and primitive square
/// as a mesh to display FTTexture on.
/// </summary>

#pragma once
#include "Components/Component.h"

#include "Core/TemplateFunctions.h"

class Actor;
class FTSprite;
class FTVertexShader;
class FTPixelShader;
class FTGeometryShader;
class FoxtrotRenderer;
class FTMaterial;
struct Mesh;

class SpriteRenderer : public Component
{
public:
	virtual FTDS::String GetName() const override { return "SpriteRenderer"; }

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void Render(FoxtrotRenderer* renderer) override;
	virtual void CloneTo(Actor* actor) override;

public:
	SpriteRenderer(
		Actor* owner,
		int	   updateOrder = DefaultVal::UPDATE_ORDER);

protected:
	FTSprite*	GetSprite() const;
	FTMaterial* GetMaterial() const;
	D3D11PSO*	GetPSO() const;

	void SetSprite(FTSprite* sprite);
	void SetMaterial(FTMaterial* mat);
	void SetPSO(D3D11PSO* pso);

private:
	FTSprite*	mSprite;
	D3D11PSO*	mPSO;
	FTMaterial* mMaterial;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;

public:
	virtual void EditorRender(FoxtrotRenderer* renderer) override;

#endif
};

namespace ChunkKey
{
	namespace SpriteRenderer
	{
		constexpr const char* SPRITE	   = "Sprite";
		constexpr const char* MATERIAL	   = "Material";
		constexpr const char* FRONT_DIR	   = "Front Dir";
		constexpr const char* SPRITE_SCALE = "Scale";
		constexpr const char* PSO		   = "SpritePSO";
	} // namespace SpriteRenderer
} // namespace ChunkKey