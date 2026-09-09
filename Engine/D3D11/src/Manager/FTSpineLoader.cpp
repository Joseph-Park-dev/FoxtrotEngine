#include "Manager/FTSpineLoader.h"

#include "spine/Extension.h"

#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "Manager/ResourceManager.h"

/// @brief Creates the Spine extension used for filesystem and allocation services.
/// @return Creates the Spine extension used for filesystem and allocation services.
spine::SpineExtension* spine::getDefaultExtension()
{
	return new spine::DefaultSpineExtension();
}

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using namespace spine;
	/// @brief Parses a Spine skeleton description using the associated atlas.
	/// @param filename Filename used to read or write the resource.
	/// @param atlas Spine atlas associated with the skeleton or texture.
	/// @param scale Scale factor applied to the content.
	/// @return Parses a Spine skeleton description using the associated atlas.
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

	/// @brief Loads the texture required by the Spine atlas.
	/// @param page Spine atlas page whose texture is processed.
	/// @param path Filesystem path of the resource or project.
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

	/// @brief Releases the texture associated with a Spine atlas page.
	/// @param texture Texture resource used by the operation.
	void FTSpineLoader::unload(void* texture)
	{
		texture = nullptr;
	}
	/// @brief Initializes the renderer used to load Spine atlas textures.
	/// @note Initializes the :FTSpineLoader base or delegates to its constructor.
	FTSpineLoader::FTSpineLoader()
	{
		spine::SpineExtension::setInstance(spine::getDefaultExtension());
	}
} // namespace D3D11
