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
	class FTTileMap;

	class TileMapRenderer :
		public SpriteRenderer
	{
	public:
		Core::FTDS::String GetName() const override
		{
			return "TileMapRenderer";
		}
		Core::FTDS::String& GetTileMapKey();
		FTTileMap*			GetTileMap() const;

		void SetTileMapKey(Core::FTDS::String& key);
		void SetTileMap(FTTileMap* tileMap);

	public:
		virtual void Initialize() override;
		virtual void ProcessInput(Core::FTInputDevice* inputDevice) override;
		virtual void Update(float deltaTime) override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render(D3D11::D3D11Renderer* renderer) override;
		virtual void CloneTo(Core::Actor* actor) override;

	public:
		TileMapRenderer(
			Core::Plugin* plugin,
			Core::Actor*  owner,
			int			  UpdateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~TileMapRenderer() override;

	protected:
		virtual void InitializeTileMap();

	private:
		FTTileMap*		   mTileMap;
		Core::FTDS::String mTileMapKey;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		virtual void EditorUIUpdate() override;
		void		 OnConfirmUpdate();

	protected:
		void UpdateCSV();
		void UpdateCSV(Core::FTDS::String& key);
#endif
	};

	namespace ChunkKey
	{
		constexpr const char* TILEMAP_KEY = "TileMapKey";
	}
} // namespace D3D11