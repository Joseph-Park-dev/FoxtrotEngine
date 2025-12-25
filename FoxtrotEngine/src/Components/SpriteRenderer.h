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
	FTSprite* GetSprite() const;
	void	  SetSprite(FTSprite* sprite);

	FTVertexShader*	  GetVS() const;
	FTGeometryShader* GetGS() const;
	FTPixelShader*	  GetPS() const;
	FTMaterial*		  GetMaterial() const;

	void SetVS(FTVertexShader* vs);
	void SetGS(FTGeometryShader* gs);
	void SetPS(FTPixelShader* ps);
	void SetMaterial(FTMaterial* mat);

private:
	FTSprite*		  mSprite;
	FTVertexShader*	  mVS;
	FTGeometryShader* mGS;
	FTPixelShader*	  mPS;
	FTMaterial*		  mMaterial;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;
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
	} // namespace SpriteRenderer
} // namespace ChunkKey

namespace Path
{
	namespace SpriteRenderer
	{
		constexpr const char* VS = "SpriteVS.hlsl";
		constexpr const char* GS = "SpriteGS.hlsl";
		constexpr const char* PS = "SpritePS.hlsl";
	} // namespace SpriteRenderer
} // namespace Path