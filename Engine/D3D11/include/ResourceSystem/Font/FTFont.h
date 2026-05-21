#pragma once
#include <ResourceSystem/Mesh/FTMeshGroup.h>

#include "FTDS/Static/HashMap.h"

namespace D3D11
{
	using namespace Common;
	class FTTexture;
	class FTVertexShader;
	class FTPixelShader;
	class FTMaterial;
	class FontChar;
	struct FontKerning;

	class FTFont :
		public FTMeshGroup
	{
	public:
		static D3D11::ResType Type;

	public:
		void LoadFont(FTTexture* img, Common::FTResourceDef& resDef);
		void AddText(Microsoft::WRL::ComPtr<ID3D11Device>& device, Common::FTDS::String& text);

		void Render(
			Common::FTDS::String& text,
			Core::IRenderer*	  renderer,
			Core::Transform*	  transform,
			Camera*				  camInst,
			FTTexture*			  tex,
			FTVertexShader*		  vs,
			FTPixelShader*		  ps,
			FTMaterial*			  mat);

		/// @brief Updates the vertices from the text to be rendered.
		void UpdateTextVertices(
			Common::FTDS::String& text,
			Core::IRenderer*	  renderer,
			Math::FTVector2		  pos,
			Math::FTVector2		  scale,
			Math::FTVector2		  padding,
			Math::FTVector3		  color,
			float				  alpha);

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
		FTFont(Common::FTResourceDef& resDef, Core::IRenderer* renderer);
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

		Common::FTDS::HashMap<FontChar*>*	 mCharList;		// list of characters
		int									 mNumKernings;	// the number of kernings
		Common::FTDS::HashMap<FontKerning*>* mKerningsList; // list to hold kerning values

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