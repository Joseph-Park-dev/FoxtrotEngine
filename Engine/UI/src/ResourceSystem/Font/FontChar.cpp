#include "ResourceSystem/Font/FontChar.h"

#include <d3d11.h>

#include "ResourceSystem/Font/TextVertex.h"
#include "Utility/D3D11Utils.h"
#include "Renderer/IRenderer.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"

namespace D3D11
{
	/// @brief Initializes the glyph's atlas bounds and layout metrics.
	/// @param data Input payload used by the operation.
	/// @note Initializes the :FontChar base or delegates to its constructor.
	FontChar::FontChar(FontCharData& data)
		: mID(data.ID)
		, mU(data.U)
		, mV(data.V)
		, mWidthOnTex(data.WidthOnTex)
		, mHeightOnTex(data.HeightOnTex)
		, mWidthOnScreen(data.WidthOnScreen)
		, mHeightOnScreen(data.HeightOnScreen)
		, mOffsetX(data.OffsetX)
		, mOffsetY(data.OffsetY)
		, mAdvanceX(data.AdvanceX)
	{
	}

	/// @brief Reports the stored identifier used to distinguish this entry.
	/// @return The stored identifier used to distinguish this entry.
	int FontChar::ID()
	{
		return mID;
	}

	float FontChar::U()
	{
		return mU;
	}

	float FontChar::V()
	{
		return mV;
	}

	/// @brief Reports the glyph's width in its font texture.
	/// @return The glyph's width in its font texture.
	float FontChar::WidthOnTex()
	{
		return mWidthOnTex;
	}

	/// @brief Reports the glyph's height in its font texture.
	/// @return The glyph's height in its font texture.
	float FontChar::HeightOnTex()
	{
		return mHeightOnTex;
	}

	/// @brief Reports the glyph width after applying the screen-space scale.
	/// @return The glyph width after applying the screen-space scale.
	float FontChar::WidthOnScreen()
	{
		return mWidthOnScreen;
	}

	/// @brief Reports the glyph height after applying the screen-space scale.
	/// @return The glyph height after applying the screen-space scale.
	float FontChar::HeightOnScreen()
	{
		return mHeightOnScreen;
	}

	/// @brief Reports the glyph's horizontal drawing offset.
	/// @return Current offset x.
	float FontChar::OffsetX()
	{
		return mOffsetX;
	}

	/// @brief Reports the glyph's vertical drawing offset.
	/// @return Current offset y.
	float FontChar::OffsetY()
	{
		return mOffsetY;
	}

	/// @brief Reports the horizontal cursor advance after rendering this glyph.
	/// @return The horizontal cursor advance after rendering this glyph.
	float FontChar::AdvanceX()
	{
		return mAdvanceX;
	}
} // namespace D3D11
