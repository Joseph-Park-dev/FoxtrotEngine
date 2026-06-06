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
		virtual void AddText(Graphics::IRenderer* renderer, const char* text) = 0;

	protected:
		virtual void LoadFont(ITexture* img) = 0;

		////////////////
		/// Gameloop ///
		////////////////
	public:
		virtual void Render(
			const char*			 text,
			Graphics::IRenderer* renderer,
			Core::Transform*	 transform,
			Graphics::ICamera*	 camInst,
			D3D11PSO*			 pso,
			IMaterial*			 mat) = 0;

	protected:
		/// @brief Updates the vertices from the text to be rendered.
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
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override = 0;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		virtual const int	GetSize() const		  = 0;
		virtual const float GetLineHeight() const = 0;
		virtual const float GetBaseHeight() const = 0;

		virtual float GetHorizontalPadding(float paddingX) = 0;
		virtual float GetVerticalPadding(float paddingY)   = 0;

		// this will return the amount of kerning we need to use for two characters
		virtual const float GetKerning(wchar_t first, wchar_t second) const = 0;

		// this will return a FontChar given a wide character
		virtual FontChar* GetChar(wchar_t c) const = 0;
		virtual ITexture* GetFontImage() const	   = 0;

		virtual void SetFontImage(ITexture* fontImage) = 0;

	public:
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