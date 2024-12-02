#pragma once
#include <string>
#include <Windows.h>

class FTResource
{
public:
	std::string&  GetFileName() { return mFileName; }
	std::string&  GetRelativePath(){ return mRelativePath; }
	UINT		  GetRefCount() { return mIsReferenced; }
	void		  SetFileName(const std::string name) { mFileName.assign(name); }
	void		  SetRelativePath(const std::string _strPath) { mRelativePath.assign(_strPath); }
	void		  SetIsReferenced(bool val) { mIsReferenced = val; }

	void		  AddRefCount() { ++mIsReferenced; }
	void		  SubtractRefCount() { --mIsReferenced; }

protected:
	std::string mFileName;
	std::string mRelativePath;

	// The resource is used in somewhere.
	UINT		mIsReferenced;

public:
	FTResource();
	virtual ~FTResource(){}

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