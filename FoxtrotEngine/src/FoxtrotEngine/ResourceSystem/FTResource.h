#pragma once
#include <string>
#include <nlohmann/json.hpp>

class FTResource
{
public:
	//std::string  GetKey() { return mKey; }
	std::string  GetRelativePath(){ return mRelativePath; }
	//void		 SetKey(const std::string _strKey) { mKey.assign(_strKey); }
	void		 SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }

protected:
	//std::string mKey;
	std::string mRelativePath;

public:
	FTResource()
		: mRelativePath(std::string{})
	{}
	virtual ~FTResource(){}

#ifdef _DEBUG
public:
	virtual void SaveProperties(nlohmann::ordered_json& out);

#endif // _DEBUG

};