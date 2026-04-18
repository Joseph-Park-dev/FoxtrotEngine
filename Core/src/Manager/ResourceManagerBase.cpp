#include "ResourceManagerBase.h"

namespace Core
{
	void ResourceManagerBase::AddFileExtensionIfNone(Core::FTDS::String& key, const char* fileType)
	{
		if (key.RFind(fileType) < 0)
			key.Append(fileType);
	}
} // namespace Core