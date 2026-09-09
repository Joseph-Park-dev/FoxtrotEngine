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
        /// @brief Returns the owner used by this tile map renderer.
        /// @return Borrowed access to the owner.
        Core::IActor* GetOwner() override { return D3D11Component::GetOwner(); }
        /// @brief Returns the update order used by this tile map renderer.
        /// @return Current update order.
        const int GetUpdateOrder() override { return D3D11Component::GetUpdateOrder(); }
        /// @brief Returns the is initialized used by this tile map renderer.
        /// @return Current value of the is initialized flag.
        const bool GetIsInitialized() const override { return D3D11Component::GetIsInitialized(); }
        /// @brief Returns the is setup used by this tile map renderer.
        /// @return Current value of the is setup flag.
        const bool GetIsSetup() const override { return D3D11Component::GetIsSetup(); }
        /// @brief Returns the is active used by this tile map renderer.
        /// @return Current value of the is active flag.
        const bool GetIsActive() const override { return D3D11Component::GetIsActive(); }
        /// @brief Updates the is active used by subsequent operations.
        /// @param value Replacement is active.
        void SetIsActive(bool value) override { D3D11Component::SetIsActive(value); }

		static inline const char* NAME = "TileMapRenderer";
		/// @brief Returns the name used by this tile map renderer.
		/// @return Borrowed access to the name.
		virtual const char*		  GetName() override
		{
			return "TileMapRenderer";
		}

		/// @brief Returns the tile map key used by this tile map renderer.
		/// @return Borrowed access to the tile map key.
		/// @note Changes through the returned reference affect this object's stored state.
		Common::FTDS::String& GetTileMapKey();
		/// @brief Returns the tile map used by this tile map renderer.
		/// @return Borrowed access to the tile map.
		FTTileMap*			  GetTileMap() const;

		/// @brief Updates the tile map key used by subsequent operations.
		/// @param key Replacement tile map key.
		void SetTileMapKey(Common::FTDS::String& key);
		/// @brief Updates the tile map used by subsequent operations.
		/// @param tileMap Replacement tile map.
		void SetTileMap(FTTileMap* tileMap);

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Core::IActor* actor) override;

	public:
		/// @brief Initializes tile-map bindings for its owning actor.
		/// @param owner Actor or object associated with the new instance.
		/// @note Unnamed parameter (int			  UpdateOrder): reserved by this interface or unused by this implementation.
		TileMapRenderer(
			Core::IActor* owner,
			int			  UpdateOrder = Core::DefaultVal::UPDATE_ORDER);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~TileMapRenderer();

	protected:
		/// @brief Builds the tile-map data and geometry used for rendering.
		virtual void InitializeTileMap();

	private:
		FTTileMap*			 mTileMap;
		Common::FTDS::String mTileMapKey;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime);
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst);
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate(Editor::CommandHistory* chInst);

	protected:
		/// @brief Refreshes tile-map data from its CSV resource.
		void UpdateCSV();
		/// @brief Refreshes tile-map data from its CSV resource.
		/// @param key Lookup key identifying the stored entry.
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
/// @brief Creates a tile-map-rendering component associated with an actor.
/// @param actor Actor participating in this operation.
/// @return Created tile map renderer instance or resource.
D3D11_API D3D11::TileMapRenderer* CreateTileMapRenderer(Core::IActor* actor);
} // namespace D3D11
