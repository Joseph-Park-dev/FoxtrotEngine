#include "Manager/EditorShapes.h"

#include "ResourceSystem/Shape/FTRectangle.h"

namespace Editor
{
	using namespace D3D11;
	FTRectangle* EditorShapes::GetCameraRect()
	{
		return mCamRect;
	}

	void EditorShapes::SetCameraRect(FTRectangle* rect)
	{
		mCamRect = rect;
	}

	void EditorShapes::RenderCamRect(D3D11Renderer* renderer)
	{
		mCamRect->Render(renderer);
	}
} // namespace Editor