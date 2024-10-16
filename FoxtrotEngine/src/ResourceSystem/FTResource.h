#pragma once
#include <string>

#define KEY_LENGTH 50

#ifdef FOXTROT_EDITOR
#include <nlohmann/json.hpp>
#endif

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
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveProperties(nlohmann::ordered_json& out, unsigned int key);
	virtual void LoadProperties(nlohmann::ordered_json& itemTree);

	virtual void UpdateUI() {};

protected:
	void UpdateRelativePath(std::string fileExtension);
#endif // FOXTROT_EDITOR
};