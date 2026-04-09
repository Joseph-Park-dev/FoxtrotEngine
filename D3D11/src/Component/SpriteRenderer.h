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
#include "Component/D3D11Component.h"

#include "TemplateFunctions.h"

namespace Core
{
	class Actor;
	class FoxtrotRenderer;
} // namespace Core

namespace D3D11
{
	class FTSprite;
	class FTVertexShader;
	class FTPixelShader;
	class FTGeometryShader;
	class FTMaterial;
	class D3D11PSO;
	struct Mesh;

	class SpriteRenderer : public D3D11Component
	{
	public:
		virtual Core::FTDS::String GetName() const override { return "SpriteRenderer"; }

	public:
		virtual void Initialize() override;
		virtual void Setup() override;
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(Core::FoxtrotRenderer* renderer) override;
		virtual void CloneTo(Core::Actor* actor) override;

	public:
		SpriteRenderer(
			Core::Plugin* plugin,
			Core::Actor*  owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER);

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
} // namespace D3D11