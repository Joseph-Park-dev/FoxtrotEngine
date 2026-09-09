// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
namespace Graphics
{
	struct FontCharData
	{
		// the unicode id
		int ID;

		// these need to be converted to texture coordinates
		// (where 0.0 is 0 and 1.0 is textureWidth of the font)
		float U;		   // u texture coordinate
		float V;		   // v texture coordinate
		float WidthOnTex;  // width of character on texture
		float HeightOnTex; // height of character on texture

		float WidthOnScreen;  // width of character in screen coords
		float HeightOnScreen; // height of character in screen coords

		// these need to be normalized based on size of font
		float OffsetX;	// offset from current cursor pos to left side of character
		float OffsetY;	// offset from top of line to top of character
		float AdvanceX; // how far to move to right for next character
	};

	struct TextVertex;

	class FontChar
	{
	public:
		/// @brief Reports the stored identifier used to distinguish this entry.
		/// @return The stored identifier used to distinguish this entry.
		int	  ID();
		float U();
		float V();
		/// @brief Reports the glyph's width in its font texture.
		/// @return The glyph's width in its font texture.
		float WidthOnTex();
		/// @brief Reports the glyph's height in its font texture.
		/// @return The glyph's height in its font texture.
		float HeightOnTex();

		/// @brief Reports the glyph width after applying the screen-space scale.
		/// @return The glyph width after applying the screen-space scale.
		float WidthOnScreen();
		/// @brief Reports the glyph height after applying the screen-space scale.
		/// @return The glyph height after applying the screen-space scale.
		float HeightOnScreen();

		/// @brief Reports the glyph's horizontal drawing offset.
		/// @return Current offset x.
		float OffsetX();
		/// @brief Reports the glyph's vertical drawing offset.
		/// @return Current offset y.
		float OffsetY();
		/// @brief Reports the horizontal cursor advance after rendering this glyph.
		/// @return The horizontal cursor advance after rendering this glyph.
		float AdvanceX();

	public:
		/// @brief Initializes the glyph's atlas bounds and layout metrics.
		/// @param data Input payload used by the operation.
		FontChar(FontCharData& data);

	private:
		// the unicode id
		int mID;

		// these need to be converted to texture coordinates
		// (where 0.0 is 0 and 1.0 is textureWidth of the font)
		float mU;			// u texture coordinate
		float mV;			// v texture coordinate
		float mWidthOnTex;	// width of character on texture
		float mHeightOnTex; // height of character on texture

		float mWidthOnScreen;  // width of character in screen coords
		float mHeightOnScreen; // height of character in screen coords

		// these need to be normalized based on size of font
		float mOffsetX;	 // offset from current cursor pos to left side of character
		float mOffsetY;	 // offset from top of line to top of character
		float mAdvanceX; // how far to move to right for next character
	};
} // namespace Graphics
