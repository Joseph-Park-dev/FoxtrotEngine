#pragma once
#include "Manager/DebugShapes.h"

namespace Editor
{
	class EditorShapes :
		public D3D11::DebugShapes
	{
		SINGLETON(EditorShapes)
	public:
		D3D11::FTRectangle* GetCameraRect();
		void				SetCameraRect(D3D11::FTRectangle* rect);
		void				RenderCamRect(D3D11::D3D11Renderer* renderer);

	private:
		D3D11::FTRectangle* mCamRect;
	};
} // namespace Editor