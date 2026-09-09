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
		/// @brief Parses font metrics and creates the resources needed to draw glyphs.
		/// @param img Image resource used by this operation.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderRes Rendering resolution.
		void LoadFont(FTTexture* img, Common::FTResourceDef& resDef, const Math::FTVector2& renderRes);
		/// @brief Appends a text item to the font's renderable text collection.
		/// @param device Direct3D device used to create GPU resources.
		/// @param text Null-terminated text to display, log, or convert.
		void AddText(Microsoft::WRL::ComPtr<ID3D11Device>& device, Common::FTDS::String& text);

		/// @brief Submits this object's graphics work for the current frame.
		/// @param text Null-terminated text to display, log, or convert.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param transform Transform associated with the actor.
		/// @param camInst Camera supplying the view and projection for this draw.
		/// @param tex Texture resource used by the operation.
		/// @param vs Vertex shader resource.
		/// @param ps Pixel shader resource.
		/// @param mat Matrix or material used by this operation.
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
		/// @param text Null-terminated text to display, log, or convert.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param pos Position or zero-based insertion index.
		/// @param scale Scale factor applied to the content.
		/// @param padding Space around the control or geometry.
		/// @param color Color components used when rendering.
		/// @param alpha Opacity or blend weight.
		void UpdateTextVertices(
			Common::FTDS::String& text,
			Core::IRenderer*	  renderer,
			Math::FTVector2		  pos,
			Math::FTVector2		  scale,
			Math::FTVector2		  padding,
			Math::FTVector3		  color,
			float				  alpha);

	public:
		/// @brief Returns the size used by this ftfont.
		/// @return Current size.
		const int	GetSize() const;
		/// @brief Returns the line height used by this ftfont.
		/// @return Current line height.
		const float GetLineHeight() const;
		/// @brief Returns the base height used by this ftfont.
		/// @return Current base height.
		const float GetBaseHeight() const;

		/// @brief Returns the horizontal padding used by this ftfont.
		/// @param paddingX Horizontal padding.
		/// @return Current horizontal padding.
		float GetHorizontalPadding(float paddingX);
		/// @brief Returns the vertical padding used by this ftfont.
		/// @param paddingY Vertical padding.
		/// @return Current vertical padding.
		float GetVerticalPadding(float paddingY);

		// this will return the amount of kerning we need to use for two characters
		/// @brief Returns the kerning used by this ftfont.
		/// @param first Initial input value or first operand.
		/// @param second Second input operand.
		/// @return Current kerning.
		const float GetKerning(wchar_t first, wchar_t second) const;

		// this will return a FontChar given a wide character
		/// @brief Returns the char used by this ftfont.
		/// @param c Character or component value.
		/// @return Borrowed access to the char.
		FontChar*  GetChar(wchar_t c) const;
		/// @brief Returns the font image used by this ftfont.
		/// @return Borrowed access to the font image.
		FTTexture* GetFontImage() const;

		/// @brief Updates the font image used by subsequent operations.
		/// @param fontImage Replacement font image.
		void SetFontImage(FTTexture* fontImage);

	public:
		/// @brief Initializes glyph metrics, textures, and text-rendering storage.
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTFont(Common::FTResourceDef& resDef, Core::IRenderer* renderer);
		/// @brief Releases the resources managed by this instance during destruction.
		~FTFont();

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
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
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
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
