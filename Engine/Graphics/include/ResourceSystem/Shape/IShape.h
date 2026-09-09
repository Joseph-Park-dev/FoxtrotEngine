// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class that renders basic geometries.
/// </summary>

#pragma once
#include "FTMath.h"

namespace Graphics
{
	class ICamera;
	class IRenderer;

	class IShape
	{
		////////////////
		/// Gameloop ///
		////////////////
	public:
		/// @brief Uploads vertex-shader constant data.
		/// @param model Model resource associated with this object.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void UpdateVC(Math::FTMatrix4& model, Graphics::ICamera* camInst) = 0;
		/// @brief Uploads geometry-shader constant data.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void UpdateGC(Graphics::ICamera* camInst)						  = 0;
		/// @brief Uploads pixel-shader constant data.
		virtual void UpdatePC()													  = 0;

		// This is for ShapeActors (e.g. SquareActor)
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void Render(Graphics::IRenderer* renderer) = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IShape() = default;

	protected:
		/// @brief Allocates and initializes the GPU constant buffer used by this object.
		/// @param device e.g. ID3D11Device in D3D11.
		virtual void InitializeConstantBuffer(void* device) = 0;

		/// @brief Uploads the current shader parameters to the constant buffers.
		/// @param device e.g. ID3D11Device in D3D11.
		/// @param context e.g. ID3D11DeviceContext in D3D11.
		virtual void UpdateConstantBuffers(void* device, void* context) = 0;

		/// @brief Recomputes the model transform used for rendering.
		/// @param pos Position or zero-based insertion index.
		/// @param rot Rotation used by the operation.
		/// @param size Number of elements or bytes required by the operation.
		virtual void UpdateModelMatrix(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 size) = 0;
		/// @brief Recomputes the camera view matrix from its position and orientation.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void UpdateViewMatrix(Graphics::ICamera* camInst)									   = 0;
		/// @brief Recomputes the camera projection from its current lens and viewport settings.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void UpdateProjectionMatrix(Graphics::ICamera* camInst)								   = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* FTSHAPE_IS_ACTIVE = "Is Active";
	}
} // namespace Graphics
