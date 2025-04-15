#pragma once
#include "spine/spine.h"

namespace spine
{
	class FTSpineLoader : public TextureLoader
	{
	public:
		SkeletonData* ReadSkeletonJsonData(const spine::String& filename, spine::Atlas* atlas, float scale);

	// Texture loading & unloading
	public:
		virtual void load(AtlasPage& page, const String& path);

		virtual void unload(void* texture);
	};
}