// Shared header — proposed
#pragma once
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

	protected:
		// Borrowed service: Editor must not delete it.
		virtual ~IEditorResourceService() = default;
	};
} // namespace Common
#endif