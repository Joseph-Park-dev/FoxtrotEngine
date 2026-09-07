// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This renders FTTilemaps using FTTexture as a texture pack,
/// primitive square as a mesh, and FTTileMap as a .CSV source.
/// </summary>

#pragma once
#include "Component/SpriteRenderer.h"

#include <string>

#include "TemplateFunctions.h"

namespace D3D11
{
#define DEFAULT_TILE_POS 0
	using namespace Common;
	class FTTileMap;

	class TileMapRenderer :
		public SpriteRenderer
	{
	public:
        // Resolve the shared IComponent interface explicitly at the renderer boundary.
        Core::IActor* GetOwner() override { return D3D11Component::GetOwner(); }
        const int GetUpdateOrder() override { return D3D11Component::GetUpdateOrder(); }
        const bool GetIsInitialized() const override { return D3D11Component::GetIsInitialized(); }
        const bool GetIsSetup() const override { return D3D11Component::GetIsSetup(); }
        const bool GetIsActive() const override { return D3D11Component::GetIsActive(); }
        void SetIsActive(bool value) override { D3D11Component::SetIsActive(value); }

		static inline const char* NAME = "TileMapRenderer";
		virtual const char*		  GetName() override
		{
			return "TileMapRenderer";
		}

		Common::FTDS::String& GetTileMapKey();
		FTTileMap*			  GetTileMap() const;

		void SetTileMapKey(Common::FTDS::String& key);
		void SetTileMap(FTTileMap* tileMap);

	public:
		virtual void Initialize() override;
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		virtual void CloneTo(Core::IActor* actor) override;

	public:
		TileMapRenderer(
			Core::IActor* owner,
			int			  UpdateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~TileMapRenderer();

	protected:
		virtual void InitializeTileMap();

	private:
		FTTileMap*			 mTileMap;
		Common::FTDS::String mTileMapKey;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUpdate(float deltaTime);
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst);
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst);

	protected:
		void UpdateCSV();
		void UpdateCSV(Common::FTDS::String& key);
#endif
	};

	namespace ChunkKey
	{
		namespace TileMapRenderer
		{
			constexpr const char* NAME		  = "TileMapRenderer";
			constexpr const char* TILEMAP_KEY = "TileMapKey";
		} // namespace TileMapRenderer
	} // namespace ChunkKey

	#include "Plugin/D3D11Exports.h"
D3D11_API D3D11::TileMapRenderer* CreateTileMapRenderer(Core::IActor* actor);
} // namespace D3D11