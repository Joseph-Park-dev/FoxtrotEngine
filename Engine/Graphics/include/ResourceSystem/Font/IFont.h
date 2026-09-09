// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <Mesh/IModel.h>

#include "FTDS/Static/HashMap.h"

namespace Graphics
{
	using namespace Common;
	class ITexture;
	class IVertexShader;
	class IPixelShader;
	class IMaterial;
	class FontChar;
	struct FontKerning;

	struct FontData
	{
		int Size;			 // size of font, lineheight and baseheight will be based on this
							 // as if this is a single unit (1.0)
		float LineHeight;	 // how far to move down to next line, will be normalized
		float BaseHeight;	 // height of all characters, will be normalized
		int	  NumCharacters; // number of characters in the font

		Common::FTDS::HashMap<FontChar*>*	 CharList;	   // list of characters
		int									 NumKernings;  // the number of kernings
		Common::FTDS::HashMap<FontKerning*>* KerningsList; // list to hold kerning values

		float	  LeftPadding;
		float	  TopPadding;
		float	  RightPadding;
		float	  BottomPadding;
		ITexture* FontImage;
	};

	class IFont :
		public IModel
	{
		//////////////////////
		/// Initialization ///
		//////////////////////
	public:
		/// @brief Appends a text item to the font's renderable text collection.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param text Null-terminated text to display, log, or convert.
		virtual void AddText(Graphics::IRenderer* renderer, const char* text) = 0;

	protected:
		/// @brief Parses font metrics and creates the resources needed to draw glyphs.
		/// @param img Image resource used by this operation.
		virtual void LoadFont(ITexture* img) = 0;

		////////////////
		/// Gameloop ///
		////////////////
	public:
		/// @brief Submits this object's graphics work for the current frame.
		/// @param text Null-terminated text to display, log, or convert.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param pso Pipeline state object used for rendering.
		/// @param mat Matrix or material used by this operation.
		virtual void Render(
			const char*			 text,
			Graphics::IRenderer* renderer,
			Core::Transform*	 transform,
			Graphics::ICamera*	 camInst,
			D3D11PSO*			 pso,
			IMaterial*			 mat) = 0;

	protected:
		/// @brief Updates the vertices from the text to be rendered.
		/// @param text Null-terminated text to display, log, or convert.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param pos Position or zero-based insertion index.
		/// @param scale Scale factor applied to the content.
		/// @param padding Space around the control or geometry.
		/// @param color Color components used when rendering.
		/// @param alpha Opacity or blend weight.
		virtual void UpdateTextVertices(
			const char*			 text,
			Graphics::IRenderer* renderer,
			Math::FTVector2		 pos,
			Math::FTVector2		 scale,
			Math::FTVector2		 padding,
			Math::FTVector3		 color,
			float				 alpha) = 0;

		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Returns the size used by this ifont.
		/// @return Current size.
		virtual const int	GetSize() const		  = 0;
		/// @brief Returns the line height used by this ifont.
		/// @return Current line height.
		virtual const float GetLineHeight() const = 0;
		/// @brief Returns the base height used by this ifont.
		/// @return Current base height.
		virtual const float GetBaseHeight() const = 0;

		/// @brief Returns the horizontal padding used by this ifont.
		/// @param paddingX Horizontal padding.
		/// @return Current horizontal padding.
		virtual float GetHorizontalPadding(float paddingX) = 0;
		/// @brief Returns the vertical padding used by this ifont.
		/// @param paddingY Vertical padding.
		/// @return Current vertical padding.
		virtual float GetVerticalPadding(float paddingY)   = 0;

		// this will return the amount of kerning we need to use for two characters
		/// @brief Returns the kerning used by this ifont.
		/// @param first Initial input value or first operand.
		/// @param second Second input operand.
		/// @return Current kerning.
		virtual const float GetKerning(wchar_t first, wchar_t second) const = 0;

		// this will return a FontChar given a wide character
		/// @brief Returns the char used by this ifont.
		/// @param c Character or component value.
		/// @return Borrowed access to the char.
		virtual FontChar* GetChar(wchar_t c) const = 0;
		/// @brief Returns the font image used by this ifont.
		/// @return Borrowed access to the font image.
		virtual ITexture* GetFontImage() const	   = 0;

		/// @brief Updates the font image used by subsequent operations.
		/// @param fontImage Replacement font image.
		virtual void SetFontImage(ITexture* fontImage) = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~IFont() override = 0;
	};

	namespace ChunkKey
	{
		namespace FTFont
		{
			constexpr const char* FTFONT	 = "FTFont";
			constexpr const char* FONT_IMAGE = "Font Image";
		} // namespace FTFont
	} // namespace ChunkKey
} // namespace Graphics
