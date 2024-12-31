// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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