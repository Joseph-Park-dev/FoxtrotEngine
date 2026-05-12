#include "Manager/FTSpineLoader.h"

#include "spine/Extension.h"

#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "Manager/ResourceManager.h"

spine::SpineExtension* spine::getDefaultExtension()
{
	return new spine::DefaultSpineExtension();
}

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using namespace spine;
	SkeletonData* FTSpineLoader::ReadSkeletonJsonData(const String& filename, Atlas* atlas, float scale)
	{
		SkeletonJson json(atlas);
		json.setScale(scale);
		SkeletonData* skeletonData = json.readSkeletonDataFile(filename);
		if (!skeletonData)
		{
			printf("%s\n", json.getError().buffer());
			exit(0);
		}
		return skeletonData;
	}

	void FTSpineLoader::load(AtlasPage& page, const String& path)
	{
		D3D11::FTTexture*	 texture  = nullptr;
		Common::FTDS::String fileName = ExtractFileName(path.buffer());
		fileName.ExtractFromLast("/");

		texture = D3D11::ResourceManager::GetInstance()->GetSprite(fileName)->GetTexture();
		if (!texture)
			return;

		/*if (page.magFilter == TextureFilter_Linear) texture->setSmooth(true);
		if (page.uWrap == TextureWrap_Repeat && page.vWrap == TextureWrap_Repeat) texture->setRepeated(true);*/

		page.texture = texture;
	}

	void FTSpineLoader::unload(void* texture)
	{
		texture = nullptr;
	}
	FTSpineLoader::FTSpineLoader()
	{
		spine::SpineExtension::setInstance(spine::getDefaultExtension());
	}
} // namespace D3D11