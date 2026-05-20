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
	using namespace Common;
	class FTTileMap;

	class TileMapRenderer :
		public SpriteRenderer
	{
	public:
		const char* GetName() const override
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
		virtual void Render(D3D11::D3D11Renderer* renderer, D3D11::Camera* camInst) override;
		virtual void CloneTo(Core::IActor* actor) override;

	public:
		TileMapRenderer(
			Core::IActor* owner,
			int			  UpdateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~TileMapRenderer() override;

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
		virtual void EditorUIUpdate() override;
		void		 OnConfirmUpdate();

	protected:
		void UpdateCSV();
		void UpdateCSV(Common::FTDS::String& key);
#endif
	};

	namespace ChunkKey
	{
		constexpr const char* TILEMAP_KEY = "TileMapKey";
	}
} // namespace D3D11