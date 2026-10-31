// Shared header — proposed
#pragma once
#include <fstream>
#include "Renderer/IRenderer.h"

#ifdef FOXTROT_EDITOR
namespace Core
{
	class IEditorResourceService
	{
	public:
		static constexpr const char* INTERFACE_NAME =
			"EditorResourceService.v1";

		virtual bool LoadDefaultResources(
			const char*			 projDirectory,
			Graphics::IRenderer* renderer = nullptr) noexcept = 0;

		virtual bool LoadAllResourcesInAsset(
			const char*			 assetDirectory,
			Graphics::IRenderer* renderer = nullptr) noexcept = 0;

		virtual void DisplayLoadedResources() = 0;

		/// @brief Writes this service's referenced resources into the open chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		virtual void SaveReferencedResources(std::ofstream& ofs) = 0;
		virtual void LoadReferencedResources(std::ifstream& ifs) = 0;

		protected :
		// Borrowed service: Editor must not delete it.
		virtual ~IEditorResourceService() = default;
	};
} // namespace Core
#endif