#pragma once
#include <string>
#include <nlohmann/json.hpp>

#define KEY_LENGTH 50

class FTResource
{
public:
	//std::string  GetKey() { return mKey; }
	std::string  GetRelativePath(){ return mRelativePath; }
	//void		 SetKey(const std::string key) { mKey.assign(key); }
	void		 SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }

protected:
	//std::string mKey;
	std::string mRelativePath;

public:
	FTResource()
		: mRelativePath{}
		//mKey{}
		
	{}
	virtual ~FTResource(){}

#ifdef _DEBUG
public:
	virtual void SaveProperties(nlohmann::ordered_json& out);

	virtual void UpdateUI(std::string& key) = 0;

protected:
	void UpdateKey(std::string& key);
	void UpdateRelativePath(std::string fileExtension);

#endif // _DEBUG

};