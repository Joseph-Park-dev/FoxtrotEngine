#pragma once
#include <string>
#include <Windows.h>

class FTResource
{
public:
	std::string&  GetFileName() { return mFileName; }
	std::string&  GetRelativePath(){ return mRelativePath; }

	void		  SetFileName(const std::string name) { mFileName.assign(name); }
	void		  SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }

protected:
	std::string mFileName;
	std::string mRelativePath;

public:
			 FTResource();
	virtual ~FTResource();

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key);
	virtual UINT LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() {};

protected:
	void UpdateRelativePath(std::string fileExtension);
#endif // FOXTROT_EDITOR
};