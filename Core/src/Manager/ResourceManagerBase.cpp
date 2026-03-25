#include "ResourceManagerBase.h"

void ResourceManagerBase::AddFileExtensionIfNone(FTDS::String& key, const char* fileType)
{
	if (key.RFind(fileType) < 0)
		key.Append(fileType);
}
