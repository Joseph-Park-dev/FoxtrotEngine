#pragma once
#include <string>
#include <nlohmann/json.hpp>

#define KEY_LENGTH 50

class FTResource
{
public:
	std::string  GetFileName() { return mFileName; }
	std::string  GetRelativePath(){ return mRelativePath; }
	void		 SetFileName(const std::string name) { mFileName.assign(name); }
	void		 SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }

protected:
	//std::string mKey;
	std::string mFileName;
	std::string mRelativePath;

public:
	FTResource()
		: mFileName{}
		, mRelativePath{}
		//mKey{}
		
	{}
	virtual ~FTResource(){}


public:
	virtual void SaveProperties(nlohmann::ordered_json& out, UINT key);
	virtual void LoadProperties(nlohmann::ordered_json& itemTree);

	virtual void UpdateUI() {};

protected:
	void UpdateRelativePath(std::string fileExtension);

 // _DEBUG

};