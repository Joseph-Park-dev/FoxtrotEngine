#pragma once
#include "spine/spine.h"

namespace D3D11
{
	class FTSpineLoader : public spine::TextureLoader
	{
	public:
		/// @brief Parses a Spine skeleton description using the associated atlas.
		/// @param filename Filename used to read or write the resource.
		/// @param atlas Spine atlas associated with the skeleton or texture.
		/// @param scale Scale factor applied to the content.
		/// @return Parses a Spine skeleton description using the associated atlas.
		spine::SkeletonData* ReadSkeletonJsonData(const spine::String& filename, spine::Atlas* atlas, float scale);

		// Texture loading & unloading
	public:
		/// @brief Loads the texture required by the Spine atlas.
		/// @param page Spine atlas page whose texture is processed.
		/// @param path Filesystem path of the resource or project.
		virtual void load(spine::AtlasPage& page, const spine::String& path);
		/// @brief Releases the texture associated with a Spine atlas page.
		/// @param texture Texture resource used by the operation.
		virtual void unload(void* texture);

	public:
		/// @brief Initializes the renderer used to load Spine atlas textures.
		FTSpineLoader();
	};
} // namespace spine
