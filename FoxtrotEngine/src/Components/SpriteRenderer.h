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
#include "Components/MeshRenderer.h"

#include "Core/TemplateFunctions.h"

class Actor;
class FTTexture;
class FoxtrotRenderer;
struct Mesh;

class SpriteRenderer : public MeshRenderer
{
public:
	virtual std::string GetName() const override { return "SpriteRenderer"; }

public:
	int GetTexWidth();
	int GetTexHeight();

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void CloneTo(Actor* actor) override;

public:
	SpriteRenderer(
		Actor* owner,
		int	   updateOrder = DefaultVal::UPDATE_ORDER);

private:
	// These fields need to be loaded from .chunk file.
	int		  mChannel;
	FTVector2 mTexScale;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUpdate(float deltaTime) override;

public:
	virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* CHANNEL	   = "Channel";
	constexpr const char* SPRITE_SCALE = "Scale";
} // namespace ChunkKey