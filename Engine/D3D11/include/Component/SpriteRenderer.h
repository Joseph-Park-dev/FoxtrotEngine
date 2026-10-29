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
#include "Component/IComponent.h"

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
		public D3D11Component
	{
	public:
		// Resolve the shared IComponent interface explicitly at the renderer boundary.
		/// @brief Returns the owner used by this sprite renderer.
		/// @return Borrowed access to the owner.
		Common::IActor* GetOwner() override;

		static inline const char* NAME = "SpriteRenderer";
		/// @brief Returns the name used by this sprite renderer.
		/// @return Borrowed access to the name.
		virtual const char* GetName() override
		{
			return "SpriteRenderer";
		}

		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() override;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup() override;

		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// /////////////
		/// Gameloop ///
		/// /////////////
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime) override;
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) override;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) override;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

		//////////////////////////////
		/// Constructors / Copying ///
		//////////////////////////////
	public:
		/// @brief Initializes sprite and material bindings for its actor.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		SpriteRenderer(Common::IActor* owner);

		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Common::IActor* actor) override;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	protected:
		/// @brief Returns the sprite used by this sprite renderer.
		/// @return Borrowed access to the sprite.
		FTSprite* GetSprite() const;
		/// @brief Returns the material used by this sprite renderer.
		/// @return Borrowed access to the material.
		FTMaterial* GetMaterial() const;

		/// @brief Updates the sprite used by subsequent operations.
		/// @param sprite Replacement sprite.
		void SetSprite(FTSprite* sprite);
		/// @brief Updates the material used by subsequent operations.
		/// @param mat Replacement material.
		void SetMaterial(FTMaterial* mat);

	private:
		Common::IActor* mOwner;

		FTSprite*		mSprite;
		FTMaterial*		mMaterial;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime);
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst);
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate() override;

	protected:
		/// @brief Exposes the sprite resource selected for this renderer.
		/// @return Borrowed access to the sprite.
		/// @note Changes through the returned reference affect this object's stored state.
		FTSprite*& Sprite();
		/// @brief Exposes the material used when drawing this object.
		/// @return Borrowed access to the material.
		/// @note Changes through the returned reference affect this object's stored state.
		FTMaterial*& Material();

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
	/// @brief Creates a sprite-rendering component associated with an actor.
	/// @param actor Actor participating in this operation.
	/// @return Created sprite renderer instance or resource.
	D3D11_API D3D11::SpriteRenderer* CreateSpriteRenderer(Core::IActor* actor);
} // namespace D3D11
