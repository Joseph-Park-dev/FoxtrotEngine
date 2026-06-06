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
		virtual void UpdateVC(Math::FTMatrix4& model, Graphics::ICamera* camInst) = 0;
		virtual void UpdateGC(Graphics::ICamera* camInst)						  = 0;
		virtual void UpdatePC()													  = 0;

		// This is for ShapeActors (e.g. SquareActor)
		virtual void Render(Graphics::IRenderer* renderer) = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		virtual ~IShape() = default;

	protected:
		/// @param device e.g. ID3D11Device in D3D11.
		virtual void InitializeConstantBuffer(void* device) = 0;

		/// @param device e.g. ID3D11Device in D3D11.
		/// @param context e.g. ID3D11DeviceContext in D3D11.
		virtual void UpdateConstantBuffers(void* device, void* context) = 0;

		virtual void UpdateModelMatrix(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 size) = 0;
		virtual void UpdateViewMatrix(Graphics::ICamera* camInst)									   = 0;
		virtual void UpdateProjectionMatrix(Graphics::ICamera* camInst)								   = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* FTSHAPE_IS_ACTIVE = "Is Active";
	}
} // namespace Graphics