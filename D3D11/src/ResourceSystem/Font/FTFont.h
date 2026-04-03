#pragma once
#include <ResourceSystem/Mesh/FTMeshGroup.h>

#include "Static/HashMap.h"

class FTTexture;
class FTVertexShader;
class FTPixelShader;
class FTMaterial;
class FontChar;
struct FontKerning;

namespace D3D11
{
	class FTFont :
		public FTMeshGroup
	{
	public:
		void LoadFont(FTTexture* img, FTResourceDef& resDef);
		void AddText(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTDS::String& text);

		void Render(
			FTDS::String&	text,
			D3D11Renderer*	renderer,
			Transform*		transform,
			Camera*			camInst,
			FTTexture*		tex,
			FTVertexShader* vs,
			FTPixelShader*	ps,
			FTMaterial*		mat);

		/// @brief Updates the vertices from the text to be rendered.
		void UpdateTextVertices(FTDS::String& text, D3D11Renderer* renderer, FTVector2 pos, FTVector2 scale, FTVector2 padding, FTVector3 color, float alpha);

	public:
		const int	GetSize() const;
		const float GetLineHeight() const;
		const float GetBaseHeight() const;

		float GetHorizontalPadding(float paddingX);
		float GetVerticalPadding(float paddingY);

		// this will return the amount of kerning we need to use for two characters
		const float GetKerning(wchar_t first, wchar_t second) const;

		// this will return a FontChar given a wide character
		FontChar*  GetChar(wchar_t c) const;
		FTTexture* GetFontImage() const;

		void SetFontImage(FTTexture* fontImage);

	public:
		FTFont(FTResourceDef& resDef, D3D11Renderer* renderer);
		~FTFont();

	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

	private:
		int mSize;			  // size of font, lineheight and baseheight will be based on this
							  // as if this is a single unit (1.0)
		float mLineHeight;	  // how far to move down to next line, will be normalized
		float mBaseHeight;	  // height of all characters, will be normalized
		int	  mNumCharacters; // number of characters in the font

		FTDS::HashMap<FontChar*>*	 mCharList;		// list of characters
		int							 mNumKernings;	// the number of kernings
		FTDS::HashMap<FontKerning*>* mKerningsList; // list to hold kerning values

		float	   mLeftPadding;
		float	   mTopPadding;
		float	   mRightPadding;
		float	   mBottomPadding;
		FTTexture* mFontImage;

#ifdef FOXTROT_EDITOR
	public:
		void UpdateUI() override;

#endif // FOXTROT_EDITOR
	};

	namespace ChunkKey
	{
		namespace FTFont
		{
			constexpr const char* FTFONT	 = "FTFont";
			constexpr const char* FONT_IMAGE = "Font Image";
		} // namespace FTFont
	} // namespace ChunkKey
} // namespace D3D11