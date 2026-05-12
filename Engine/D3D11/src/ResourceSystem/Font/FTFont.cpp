#include "ResourceSystem/Font/FTFont.h"

#include "Renderer/D3D11Renderer.h"
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

	void FTFont::Render(Common::FTDS::String& text, D3D11Renderer* renderer, Core::Transform* transform, Camera* camInst, FTTexture* tex, FTVertexShader* vs, FTPixelShader* ps, FTMaterial* mat)
	{
		if (!Meshes())
			return;

		FTVector3 pos	= transform->GetWorldPosition();
		FTVector3 scale = transform->GetWorldScale();

		UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext(), transform, camInst, mat, GetFrontDir());

		if (!vs || !ps) // Vertex Shader is always required when drawing.
			return;

		UINT						 stride	 = sizeof(TextVertex);
		UINT						 offset	 = 0;
		ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

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

	void FTFont::UpdateTextVertices(Common::FTDS::String& text, D3D11Renderer* renderer, FTVector2 pos, FTVector2 scale, FTVector2 padding, FTVector3 color, float alpha)
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
		renderer->GetContext()->Map(Meshes()->At(0)->VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

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
			if (numCharacters >= BufferSize::MAX_FONT_CHAR_COUNT)
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
		renderer->GetContext()->Unmap(Meshes()->At(0)->VertexBuffer.Get(), 0);
	}

	const int FTFont::GetSize() const
	{
		return mSize;
	}

	const float FTFont::GetLineHeight() const
	{
		return 0.0f;
	}

	const float FTFont::GetBaseHeight() const
	{
		return 0.0f;
	}

	float FTFont::GetHorizontalPadding(float paddingX)
	{
		return (mLeftPadding + mRightPadding) * paddingX;
	}

	float FTFont::GetVerticalPadding(float paddingY)
	{
		return (mTopPadding + mBottomPadding) * paddingY;
	}

	FTFont::FTFont(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
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

		metaPath.SubStr(0, metaPath.RFind(FileTypes::FONT));
		metaPath.Append(FileTypes::FONT_META);

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

		CreateTextureSampler(renderer->GetDevice());
		InitializeConstantBuffers(renderer->GetDevice());

		if (mFontImage)
			LoadFont(mFontImage, resDef);
	}

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

	void FTFont::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTFont::FTFONT);

		if (mFontImage)
			FileIOHelper::SaveString(ofs, ChunkKey::FTFont::FONT_IMAGE, mFontImage->GetFileName());
		else
			FileIOHelper::SaveString(ofs, ChunkKey::FTFont::FONT_IMAGE, Common::ChunkKey::NullVal::NULL_OBJECT);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTFont::FTFONT);
	}

	void FTFont::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String key;
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTFont::FTFONT);
		FileIOHelper::LoadBasicString(ifs, key);

		mFontImage = D3D11::ResourceManager::GetInstance()->GetSprite(key)->GetTexture();
	}

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

	FontChar* FTFont::GetChar(wchar_t c) const
	{
		return mCharList->At(c)->Value();
	}

	FTTexture* FTFont::GetFontImage() const
	{
		return mFontImage;
	}

	void FTFont::SetFontImage(FTTexture* fontImage)
	{
		mFontImage = fontImage;
	}

	void FTFont::LoadFont(FTTexture* img, Common::FTResourceDef& resDef)
	{
		FTVector2 renderRes = Camera::GetInstance()->GetResolution();

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