#include "FTSpineLoader.h"

#include "spine/Extension.h"

#include "ResourceSystem/FTTexture.h"
#include "Managers/ResourceManager.h"
#include "FTSpineLoader.h"

#ifdef FOXTROT_EDITOR
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

spine::SkeletonData* spine::FTSpineLoader::ReadSkeletonJsonData(const spine::String& filename, spine::Atlas* atlas, float scale)
{
	spine::SkeletonJson json(atlas);
	json.setScale(scale);
	spine::SkeletonData* skeletonData = json.readSkeletonDataFile(filename);
	if (!skeletonData)
	{
		printf("%s\n", json.getError().buffer());
		exit(0);
	}
	return skeletonData;
}

void spine::FTSpineLoader::load(AtlasPage& page, const String& path)
{
	FTTexture*	 texture  = nullptr;
	FTDS::String fileName = ExtractFileName(path.buffer());
	fileName.ExtractFromLast("/");
	texture = EditorResourceManager::GetInstance()->GetLoadedTexture(fileName);

	if (!texture)
		return;

	/*if (page.magFilter == TextureFilter_Linear) texture->setSmooth(true);
	if (page.uWrap == TextureWrap_Repeat && page.vWrap == TextureWrap_Repeat) texture->setRepeated(true);*/

	page.texture = texture;
}

void spine::FTSpineLoader::unload(void* texture)
{
	texture = nullptr;
}

spine::SpineExtension* spine::getDefaultExtension()
{
	return new DefaultSpineExtension();
}
