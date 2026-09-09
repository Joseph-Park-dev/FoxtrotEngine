#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Font/FTFont.h"

#include "Renderer/IRenderer.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/FTRectArea.h"
#include "Utility/D3D11Utils.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Font/TextVertex.h"
#include "ResourceSystem/Font/FontChar.h"
#include "ResourceSystem/Font/FontKerning.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/BufferSizes.h"
#include "Renderer/Camera.h"
#include "Actor/Transform.h"
#include "FileSystem/FileTypes.h"

#include "FTDS/Static/FTString.h"

using Microsoft::WRL::ComPtr;

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using namespace Math;
	ResType FTFont::Type = ResType::FONT;

	/// @brief Appends a text item to the font's renderable text collection.
	/// @param device Direct3D device used to create GPU resources.
	/// @param text Null-terminated text to display, log, or convert.
	void FTFont::AddText(ComPtr<ID3D11Device>& device, Common::FTDS::String& text)
	{
		Common::FTDS::DynamicArray<TextVertex> vertices;
		vertices.Reserve(text.GetLength());

		for (size_t i = 0; i < text.GetLength(); ++i)
			vertices.PushBack(TextVertex(0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f));

		Mesh* mesh = DBG_NEW Mesh;
		D3D11Utils::CreateVertexBuffer(device, vertices, mesh->VertexBuffer);

		mesh->VertexCount = static_cast<UINT>(text.GetLength());
		Meshes()->Reserve(Meshes()->GetSize() + 1);
		Meshes()->PushBack(mesh);
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param text Null-terminated text to display, log, or convert.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param transform Transform associated with the actor.
	/// @param camInst Camera supplying the view and projection for this draw.
	/// @param tex Texture resource used by the operation.
	/// @param vs Vertex shader resource.
	/// @param ps Pixel shader resource.
	/// @param mat Matrix or material used by this operation.
	void FTFont::Render(Common::FTDS::String& text, Core::IRenderer* renderer, Core::Transform* transform, Camera* camInst, FTTexture* tex, FTVertexShader* vs, FTPixelShader* ps, FTMaterial* mat)
	{
		if (!Meshes())
			return;

		FTVector3 pos	= transform->GetWorldPosition();
		FTVector3 scale = transform->GetWorldScale();

		UpdateConstantBuffers(static_cast<D3D11Renderer*>(renderer)->GetDevice(), static_cast<D3D11Renderer*>(renderer)->GetContext(), transform, camInst, mat, GetFrontDir());

		if (!vs || !ps) // Vertex Shader is always required when drawing.
			return;

		UINT						 stride	 = sizeof(TextVertex);
		UINT						 offset	 = 0;
		ComPtr<ID3D11DeviceContext>& context = static_cast<D3D11Renderer*>(renderer)->GetContext();

		size_t size = Meshes()->GetSize();

		Meshes()->IterateArray([&](Mesh* mesh) {
			if (!mesh)
				return;

			context->VSSetConstantBuffers(0, 1, GetVCBuf().GetAddressOf());

			if (tex)
			{
				std::vector<ID3D11ShaderResourceView*> resViews;
				resViews.push_back(tex->GetSRV().Get());
				context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
			}

			context->VSSetShader(vs->GetShader().Get(), 0, 0);
			context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
			context->PSSetShader(ps->GetShader().Get(), 0, 0);

			if (mat)
				context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

			context->IASetInputLayout(vs->GetInputLayout().Get());
			context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
			// context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			context->DrawInstanced(4, static_cast<UINT>(text.GetLength()), 0, 0);
		});
	}

	/// @brief Rebuilds glyph vertices for the currently stored text.
	/// @param text Null-terminated text to display, log, or convert.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param pos Position or zero-based insertion index.
	/// @param scale Scale factor applied to the content.
	/// @param padding Space around the control or geometry.
	/// @param color Color components used when rendering.
	/// @param alpha Opacity or blend weight.
	void FTFont::UpdateTextVertices(Common::FTDS::String& text, Core::IRenderer* renderer, FTVector2 pos, FTVector2 scale, FTVector2 padding, FTVector3 color, float alpha)
	{
		if (text.IsEmpty())
			return;

		size_t numCharacters = 0;

		float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
		float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;

		float x = topLeftScreenX;
		float y = topLeftScreenY;

		float horrizontalPadding =
			(mLeftPadding + mRightPadding) * padding.x;
		float verticalPadding = (mTopPadding + mBottomPadding) * padding.y;

		wchar_t lastChar = -1; // no last character to start with

		D3D11_MAPPED_SUBRESOURCE mapped;
		static_cast<D3D11Renderer*>(renderer)->GetContext()->Map(Meshes()->At(0)->VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

		for (size_t i = 0; i < text.GetLength(); ++i)
		{
			wchar_t c = text.At(i);

			FontChar* fc = this->GetChar(c);

			// character not in font char set
			if (fc == nullptr)
				continue;

			// end of string
			if (c == L'\0')
				break;

			// new line
			if (c == L'\n')
			{
				x = topLeftScreenX;
				y -= (mLineHeight + verticalPadding) * scale.y;
				continue;
			}

			// don't overflow the buffer. In your app if this is true, you can
			// implement a resize of your text vertex buffer
			if (numCharacters >= Common::BufferSize::MAX_FONT_CHAR_COUNT)
				break;

			float kerning = 0.0f;
			if (i > 0)
				kerning = this->GetKerning(lastChar, c);

			TextVertex* vertices = (TextVertex*)mapped.pData;

			size_t size = sizeof(vertices) / sizeof(TextVertex*);

			/*for (size_t j = 0; j < 4; j++)
			{
				size_t idx = 4 * i + j;
				vertices[i][j].pos		= DirectX::XMFLOAT4(x + ((fc->OffsetX() + kerning) * scale.x), y - (fc->OffsetY() * scale.y), fc->WidthOnScreen() * scale.x, fc->HeightOnScreen() * scale.y);
				vertices[i][j].color	= DirectX::XMFLOAT4(color.x, color.y, color.z, alpha);
				vertices[i][j].texCoord = DirectX::XMFLOAT4(fc->U(), fc->V(), fc->WidthOnTex(), fc->HeightOnTex());
			}*/

			vertices[i].pos		 = FTVector4(x + ((fc->OffsetX() + kerning) * scale.x), y - (fc->OffsetY() * scale.y), fc->WidthOnScreen() * scale.x, fc->HeightOnScreen() * scale.y);
			vertices[i].color	 = FTVector4(color.x, color.y, color.z, alpha);
			vertices[i].texCoord = FTVector4(fc->U(), fc->V(), fc->WidthOnTex(), fc->HeightOnTex());

			++numCharacters;

			// remove horrizontal padding and advance to next char position
			x += (fc->AdvanceX() - horrizontalPadding) * scale.x;

			lastChar = c;
		}
		static_cast<D3D11Renderer*>(renderer)->GetContext()->Unmap(Meshes()->At(0)->VertexBuffer.Get(), 0);
	}

	/// @brief Returns the size used by this ftfont.
	/// @return Current size.
	const int FTFont::GetSize() const
	{
		return mSize;
	}

	/// @brief Returns the line height used by this ftfont.
	/// @return Current line height.
	const float FTFont::GetLineHeight() const
	{
		return 0.0f;
	}

	/// @brief Returns the base height used by this ftfont.
	/// @return Current base height.
	const float FTFont::GetBaseHeight() const
	{
		return 0.0f;
	}

	/// @brief Returns the horizontal padding used by this ftfont.
	/// @param paddingX Horizontal padding.
	/// @return Current horizontal padding.
	float FTFont::GetHorizontalPadding(float paddingX)
	{
		return (mLeftPadding + mRightPadding) * paddingX;
	}

	/// @brief Returns the vertical padding used by this ftfont.
	/// @param paddingY Vertical padding.
	/// @return Current vertical padding.
	float FTFont::GetVerticalPadding(float paddingY)
	{
		return (mTopPadding + mBottomPadding) * paddingY;
	}

	/// @brief Initializes glyph metrics, textures, and text-rendering storage.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @note Initializes the :FTFont base or delegates to its constructor.
	FTFont::FTFont(Common::FTResourceDef& resDef, Core::IRenderer* renderer)
		: FTMeshGroup(resDef, renderer, nullptr)
		, mSize(0)
		, mLineHeight(0.f)
		, mBaseHeight(0.f)
		, mNumCharacters(0)
		, mCharList(DBG_NEW Common::FTDS::HashMap<FontChar*>)
		, mNumKernings(0)
		, mKerningsList(DBG_NEW Common::FTDS::HashMap<FontKerning*>)
		, mLeftPadding(0.f)
		, mTopPadding(0.f)
		, mRightPadding(0.f)
		, mBottomPadding(0.f)
		, mFontImage(nullptr)
	{
		if (!mCharList->IsEmpty() && mKerningsList)
			return;

		Common::FTDS::String metaPath = resDef.Path;

		metaPath.SubStr(0, metaPath.RFind(Common::FileTypes::FONT));
		metaPath.Append(Common::FileTypes::FONT_META);

		if (!std::filesystem::exists(metaPath.C_Str()))
		{
			std::ofstream ofs(metaPath.C_Str());
			SaveProperties(ofs);
			FileIOHelper::SaveBufferToFile(ofs);
		}

		std::ifstream ifs(metaPath.C_Str());
		LoadProperties(ifs);

		if (!mFontImage)
			return;

		CreateTextureSampler(static_cast<D3D11Renderer*>(renderer)->GetDevice());
		InitializeConstantBuffers(static_cast<D3D11Renderer*>(renderer)->GetDevice());

		float x, y, width, height;
			renderer->GetViewport(x, y, width, height);
			LoadFont(mFontImage, resDef, FTVector2(width, height));
	}

	/// @brief Releases the resources managed by this instance during destruction.
	FTFont::~FTFont()
	{
		mCharList->IterateAllValues([&](FontChar* fc) {
			if (fc)
			{
				delete fc;
				fc = nullptr;
			}
		});

		mKerningsList->IterateAllValues([&](FontKerning* fk) {
			if (fk)
			{
				delete fk;
				fk = nullptr;
			}
		});

		delete mCharList;
		delete mKerningsList;
	}

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void FTFont::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTFont::FTFONT);

		if (mFontImage)
			FileIOHelper::SaveString(ofs, ChunkKey::FTFont::FONT_IMAGE, mFontImage->GetFileName());
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTFont::FONT_IMAGE, Common::ChunkKey::NullVal::NULL_OBJECT);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTFont::FTFONT);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void FTFont::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String key;
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTFont::FTFONT);
		FileIOHelper::LoadBasicString(ifs, key);

		mFontImage = D3D11::ResourceManager::GetInstance()->GetSprite(key)->GetTexture();
	}

	/// @brief Returns the kerning used by this ftfont.
	/// @param first Initial input value or first operand.
	/// @param second Second input operand.
	/// @return Current kerning.
	const float FTFont::GetKerning(wchar_t first, wchar_t second) const
	{
		for (int i = 0; i < mNumKernings; ++i)
		{
			FontKerning* kerning = mKerningsList->At(i)->Value();
			if ((wchar_t)kerning->firstid == first &&
				(wchar_t)kerning->secondid == second)
				return kerning->amount;
		}
		return 0.0f;
	}

	/// @brief Returns the char used by this ftfont.
	/// @param c Character or component value.
	/// @return Borrowed access to the char.
	FontChar* FTFont::GetChar(wchar_t c) const
	{
		return mCharList->At(c)->Value();
	}

	/// @brief Returns the font image used by this ftfont.
	/// @return Borrowed access to the font image.
	FTTexture* FTFont::GetFontImage() const
	{
		return mFontImage;
	}

	/// @brief Updates the font image used by subsequent operations.
	/// @param fontImage Replacement font image.
	void FTFont::SetFontImage(FTTexture* fontImage)
	{
		mFontImage = fontImage;
	}

	/// @brief Parses font metrics and creates the resources needed to draw glyphs.
	/// @param img Image resource used by this operation.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderRes Rendering resolution.
	void FTFont::LoadFont(FTTexture* img, Common::FTResourceDef& resDef, const Math::FTVector2& renderRes)
	{

		std::wifstream fs;
		fs.open(resDef.Path);

		std::wstring tmp;
		int			 startpos;

		// extract font name
		fs >> tmp >> tmp; // info face="Arial"
		// startpos  = tmp.find(L"\"") + 1;
		// this->name() = tmp.substr(startpos, tmp.size() - startpos - 1);

		// get font size
		fs >> tmp; // size=73
		startpos	= static_cast<int>(tmp.find(L"=") + 1);
		this->mSize = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
		fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >>
			tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1

		// get padding
		fs >> tmp; // padding=5,5,5,5
		startpos = static_cast<int>(tmp.find(L"=") + 1);
		tmp		 = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

		// get up padding
		startpos		  = static_cast<int>(tmp.find(L",") + 1);
		this->mTopPadding = std::stoi(tmp.substr(0, startpos)) / (float)renderRes.x;

		// get right padding
		tmp					= tmp.substr(startpos, tmp.size() - startpos);
		startpos			= static_cast<int>(tmp.find(L",") + 1);
		this->mRightPadding = std::stoi(tmp.substr(0, startpos)) / (float)renderRes.x;

		// get down padding
		tmp		 = tmp.substr(startpos, tmp.size() - startpos);
		startpos = static_cast<int>(tmp.find(L",") + 1);
		this->mBottomPadding =
			std::stoi(tmp.substr(0, startpos)) / (float)renderRes.x;

		// get left padding
		tmp				   = tmp.substr(startpos, tmp.size() - startpos);
		this->mLeftPadding = std::stoi(tmp) / (float)renderRes.x;

		fs >> tmp; // spacing=0,0

		fs >> tmp; // outline

		// get lineheight (how much to move down for each line), and normalize
		// (between 0.0 and 1.0 based on size of font)
		fs >> tmp >> tmp; // common lineHeight=95
		startpos = static_cast<int>(tmp.find(L"=") + 1);
		this->mLineHeight =
			(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
			(float)renderRes.y;

		// get base height (height of all characters), and normalize (between 0.0
		// and 1.0 based on size of font)
		fs >> tmp; // base=68
		startpos = static_cast<int>(tmp.find(L"=") + 1);
		;
		this->mBaseHeight =
			(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
			(float)renderRes.y;

		// get texture width
		fs >> tmp; // scaleW=512
		startpos = static_cast<int>(tmp.find(L"=") + 1);
		;
		// font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get texture height
		fs >> tmp; // scaleH=512
		startpos = static_cast<int>(tmp.find(L"=") + 1);
		;
		// font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// get pages, packed, page id
		fs >> tmp >> tmp; // pages=1 packed=0
		fs >> tmp >> tmp; // page id=0
		fs >> tmp >> tmp; //
		fs >> tmp >> tmp; //

		// get texture filename
		std::wstring wtmp;
		fs >> wtmp; // file="Arial.png"
		startpos = static_cast<int>(wtmp.find(L"\"") + 1);
		// font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

		// get number of characters
		fs >> tmp >> tmp; // chars count=97
		startpos			 = static_cast<int>(tmp.find(L"=") + 1);
		this->mNumCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// initialize the character list
		this->mCharList->Reserve(this->mNumCharacters);

		for (int c = 0; c < this->mNumCharacters; ++c)
		{
			FontCharData fc;

			// get unicode id
			fs >> tmp >> tmp; // char id=0
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.ID =
				std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get x
			fs >> tmp; // x=392
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.U =
				(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
				(float)img->GetWidth();

			// get y
			fs >> tmp; // y=340
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.V =
				(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
				(float)img->GetHeight();

			// get width
			fs >> tmp; // width=47
			startpos		 = static_cast<int>(tmp.find(L"=") + 1);
			tmp				 = tmp.substr(startpos, tmp.size() - startpos);
			fc.WidthOnScreen = (float)std::stoi(tmp) / (float)renderRes.x;
			fc.WidthOnTex =
				(float)std::stoi(tmp) / (float)img->GetWidth();

			// get height
			fs >> tmp; // height=57
			startpos		  = static_cast<int>(tmp.find(L"=") + 1);
			tmp				  = tmp.substr(startpos, tmp.size() - startpos);
			fc.HeightOnScreen = (float)std::stoi(tmp) / (float)renderRes.y;
			fc.HeightOnTex =
				(float)std::stoi(tmp) / (float)img->GetHeight();

			// get xoffset
			fs >> tmp; // xoffset=-6
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.OffsetX =
				(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
				(float)renderRes.x;

			// get yoffset
			fs >> tmp; // yoffset=16
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.OffsetY =
				(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
				(float)renderRes.y;

			// get xadvance
			fs >> tmp; // xadvance=65
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			fc.AdvanceX =
				(float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) /
				(float)renderRes.x;

			// get page
			// get channel
			fs >> tmp >> tmp; // page=0    chnl=0

			this->mCharList->Insert(fc.ID, DBG_NEW FontChar(fc));
		}

		// get number of kernings
		fs >> tmp >> tmp; // kernings count=96
		startpos		   = static_cast<int>(tmp.find(L"=") + 1);
		this->mNumKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

		// initialize the kernings list
		this->mKerningsList->Reserve(this->mNumKernings);

		for (int k = 0; k < this->mNumKernings; ++k)
		{
			FontKerning* kerning = DBG_NEW FontKerning();
			// get first character
			fs >> tmp >> tmp; // kerning first=87
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			kerning->firstid =
				std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get second character
			fs >> tmp; // second=45
			startpos = static_cast<int>(tmp.find(L"=") + 1);
			;
			kerning->secondid =
				std::stoi(tmp.substr(startpos, tmp.size() - startpos));

			// get amount
			fs >> tmp; // amount=-1
			startpos		= static_cast<int>(tmp.find(L"=") + 1);
			int t			= std::stoi(tmp.substr(startpos, tmp.size() - startpos));
			kerning->amount = (float)t / (float)renderRes.x;

			this->mKerningsList->Insert(k, kerning);
		}
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void FTFont::UpdateUI()
	{
		if (ImGui::Button("Reload font"))
		{
			/*	if (mFontImage)
					LoadFont(mFontImage);*/
		}
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11
