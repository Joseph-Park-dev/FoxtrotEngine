// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CCore.h"

int main(int argc, char *argv[])
{
	bool success = CCore::GetInstance()->Initialize();
	if (success)
	{
		CCore::GetInstance()->RunLoop();
	}
	CCore::GetInstance()->ShutDown();
	CCore::GetInstance()->Release();
	return 0;
}
