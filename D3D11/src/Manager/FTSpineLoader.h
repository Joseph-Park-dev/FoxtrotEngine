#pragma once
#include "spine/spine.h"

extern spine::SpineExtension* gSpineExtension; // defined in FTSpineLoader.cpp

namespace D3D11
{
	class FTSpineLoader : public spine::TextureLoader
	{
	public:
		spine::SkeletonData* ReadSkeletonJsonData(const spine::String& filename, spine::Atlas* atlas, float scale);

		// Texture loading & unloading
	public:
		virtual void load(spine::AtlasPage& page, const spine::String& path);
		virtual void unload(void* texture);

	public:
		FTSpineLoader();
	};
} // namespace spine