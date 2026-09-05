// GenericData.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "GenericData.h"


namespace GenericData
{
	// This is an example of an exported variable
	GENERICDATA_API int nGenericData=0;

	// This is an example of an exported function.
	GENERICDATA_API int fnGenericData(void)
	{
		return 0;
	}

	// This is the constructor of a class that has been exported.
	CGenericData::CGenericData()
	{
		return;
	}
} // namespace GenericData
