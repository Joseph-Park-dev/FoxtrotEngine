#pragma once
#include "ResourceSystem/Shape/FTShape.h"

#include <fstream>

#include <Plugin/D3D11Exports.h>

namespace D3D11
{
	namespace ProcName
	{
		constexpr const char* CREATE_FT_RECTANGLE = "CreateFTRectangle";
	}

	class FTRectangle : public FTShape
	{
	public:
		// Don't forget to DebugShape::AddShape() after this!
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void Initialize(D3D11Renderer* renderer) override;

	public:
		/// @brief Initializes rectangle geometry and its graphics resources.
		FTRectangle();
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~FTRectangle() override;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs);
		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI();
#endif
	};

	/// @brief Creates rectangle geometry for rendering.
	/// @return Created ftrectangle instance or resource.
	extern "C" D3D11_API D3D11::FTRectangle* CreateFTRectangle(); 

	namespace ChunkKey
	{
		constexpr const char* FTRECTANGLE = "FTRectangle";
	}
} // namespace D3D11
