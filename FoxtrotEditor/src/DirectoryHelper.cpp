#include "DirectoryHelper.h"

DirectoryHelper::DirectoryHelper()
	: mCurrProjectPath()
	, mCurrChunkPath()
	, mCurrChunkSaved(false)
{}

DirectoryHelper::~DirectoryHelper()
{}

std::string& DirectoryHelper::GetProjectPath()
{
	return mCurrProjectPath;
}

std::string& DirectoryHelper::GetChunkPath()
{
	return mCurrChunkPath;
}

bool DirectoryHelper::GetCurrChunkSaved() const
{
	return mCurrChunkSaved;
}

void DirectoryHelper::SetProjPath(std::string&& path)
{
	mCurrProjectPath.assign(path);
}

void DirectoryHelper::SetChunkPath(std::string&& path)
{
	mCurrChunkPath.assign(path);
}

void DirectoryHelper::SetCurrChunkSaved(bool saved)
{
	mCurrChunkSaved = saved;
}
