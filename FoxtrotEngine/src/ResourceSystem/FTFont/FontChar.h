#pragma once
class FoxtrotRenderer;

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
	int	  ID();
	float U();
	float V();
	float WidthOnTex();
	float HeightOnTex();

	float WidthOnScreen();
	float HeightOnScreen();

	float OffsetX();
	float OffsetY();
	float AdvanceX();

public:
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