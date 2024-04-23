#pragma once
#include <string>

class FTResource
{
public:
	std::string GetKey() { return mKey; }
	std::string GetRelativePath(){ return mRelativePath; }
	void		 SetKey(const std::string _strKey) { mKey.assign(_strKey); }
	void		 SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }

protected:
	std::string mKey;
	std::string mRelativePath;

public:
	FTResource(){}
	virtual ~FTResource(){}
};