//// ----------------------------------------------------------------
//// Foxtrot Engine 2D
//// Copyright (C) 2025 JungBae Park. All rights reserved.
////
//// Released under the GNU General Public License v3.0
//// See LICENSE in root directory for full details.
//// ----------------------------------------------------------------
//
// #include <../Common/include/ResourceSystem/FTResource.h>
//
// namespace Common
//{
//	FTResource::FTResource(Common::FTResourceDef& resDef)
//	{
//		mFileName->Assign(resDef.FileName);
//		mRelativePath->Assign(resDef.Path);
//	}
//
// #ifdef FOXTROT_EDITOR
//	bool FTResource::IsReferenced() const
//	{
//		return 0 < mRefCount;
//	}
//
//	void FTResource::AddRefCount()
//	{
//		++mRefCount;
//	}
//
//	void FTResource::SubtractRefCount()
//	{
//		--mRefCount;
//	}
// #endif // FOXTROT_EDITOR
//} // namespace Common