#pragma once
#include "Manager/DebugShapes.h"

namespace Editor
{
	class EditorShapes :
		public D3D11::DebugShapes
	{
		SINGLETON(EditorShapes)
	public:
		/// @brief Returns the camera rect used by this editor shapes.
		/// @return Borrowed access to the camera rect.
		D3D11::FTRectangle* GetCameraRect();
		/// @brief Updates the camera rect used by subsequent operations.
		/// @param rect Replacement camera rect.
		void				SetCameraRect(D3D11::FTRectangle* rect);
		/// @brief Draws the camera's visible region as an editor overlay.
		/// @param renderer Renderer providing the graphics device and current render state.
		void				RenderCamRect(D3D11::D3D11Renderer* renderer);

	private:
		D3D11::FTRectangle* mCamRect;
	};
} // namespace Editor
