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
#include "Graphics/ISpriteRenderer.h"

namespace Core
{
	class IActor;
	class IRenderer;
	class ICamera;
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

	class SpriteRenderer :
		public Core::ISpriteRenderer
	{
	public:
		static inline const char* NAME = "SpriteRenderer";
		virtual const char*		  GetName() override
		{
			return "SpriteRenderer";
		}

		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		virtual void Initialize() override;
		virtual void Setup() override;

		////////////////
		/// Gameloop ///
		////////////////
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

		//////////////////////////////
		/// Constructors / Copying ///
		//////////////////////////////
	public:
		SpriteRenderer(
			Core::IActor* owner,
			int			  updateOrder = Core::DefaultVal::UPDATE_ORDER);

		virtual void CloneTo(Core::IActor* actor) override;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
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

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void EditorUIUpdate() override;

#endif
	};

	namespace ChunkKey
	{
		namespace SpriteRenderer
		{
			constexpr const char* NAME = "SpriteRenderer";

			constexpr const char* SPRITE	   = "Sprite";
			constexpr const char* MATERIAL	   = "Material";
			constexpr const char* FRONT_DIR	   = "Front Dir";
			constexpr const char* SPRITE_SCALE = "Scale";
			constexpr const char* PSO		   = "SpritePSO";
		} // namespace SpriteRenderer
	} // namespace ChunkKey

#include "Plugin/D3D11Exports.h"
	D3D11_API D3D11::SpriteRenderer* CreateSpriteRenderer(Core::IActor* actor)
	{
		return DBG_NEW D3D11::SpriteRenderer(actor);
	}
} // namespace D3D11