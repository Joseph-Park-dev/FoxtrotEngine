#pragma once
#include <string>

class FTResource
{
public:
	std::wstring GetKey() { return mKey; }
	std::wstring GetRelativePath(){ return mRelativePath; }
	void		 SetKey(const std::wstring _strKey) { mKey.assign(_strKey); }
	void		 SetRelativePath(const std::wstring _strPath) { mRelativePath.assign(_strPath); }

protected:
	std::wstring mKey;
	std::wstring mRelativePath;

public:
	FTResource(){}
	virtual ~FTResource(){}
};