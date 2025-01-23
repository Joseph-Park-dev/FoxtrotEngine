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
#include "ResourceSystem/MeshData.h"

class Actor;
class FTTexture;
class FoxtrotRenderer;
struct Mesh;

class SpriteRenderer 
	: public MeshRenderer
{
public:
	int			 GetTexWidth()  { return GetTexture()->GetTexWidth() * mTexScale.x; }
	int			 GetTexHeight() { return GetTexture()->GetTexHeight() * mTexScale.y; }

	std::string  GetName()		const override{
		return	"SpriteRenderer";
	}

public:
	virtual	void Initialize		(FTCore* coreInstance)		override;
	virtual void CloneTo(Actor* actor)	override;

protected:
	virtual bool InitializeMesh	()		override;

public:
			SpriteRenderer(
				Actor* owner, 
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
	virtual void EditorUIUpdate()		override;

protected:
	virtual void OnConfirmUpdate()		override;
			void OnResetTexture();
			void UpdateSprite();
			void UpdateSprite(UINT& key);
#endif // FOXTROT_EDITOR
};