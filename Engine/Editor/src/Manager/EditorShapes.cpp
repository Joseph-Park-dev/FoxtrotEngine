#include "Manager/EditorShapes.h"

#include "ResourceSystem/Shape/FTRectangle.h"

namespace Editor
{
	using namespace D3D11;
	/// @brief Returns the camera rect used by this editor shapes.
	/// @return Borrowed access to the camera rect.
	FTRectangle* EditorShapes::GetCameraRect()
	{
		return mCamRect;
	}

	/// @brief Updates the camera rect used by subsequent operations.
	/// @param rect Replacement camera rect.
	void EditorShapes::SetCameraRect(FTRectangle* rect)
	{
		mCamRect = rect;
	}

	/// @brief Draws the camera's visible region as an editor overlay.
	/// @param renderer Renderer providing the graphics device and current render state.
	void EditorShapes::RenderCamRect(D3D11Renderer* renderer)
	{
		mCamRect->Render(renderer);
	}
} // namespace Editor
namespace Editor { /** @brief Initializes the editor overlay geometry.
 * @note Initializes the :EditorShapes base or delegates to its constructor. */ EditorShapes::EditorShapes() : mCamRect(nullptr) {} }

namespace Editor { /** @brief Completes destruction through the object's inheritance hierarchy. */ EditorShapes::~EditorShapes() = default; }
