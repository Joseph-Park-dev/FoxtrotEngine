// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Macros that makes class into singleton
/// </summary>

#pragma once
#include "Debugging/DebugMemAlloc.h"

// Makes a classe into singleton.
// Don't forget to call Destory() to delete mObject!
#define SINGLETON(TYPE) public:\
							static TYPE* GetInstance() \
								{ \
									if(mInstance == nullptr) \
										mInstance = DBG_NEW TYPE(); \
									return mInstance; \
								} \
							static void Destroy() \
								{ \
									if (mInstance) \
									{ \
										delete mInstance; \
										mInstance = nullptr; \
									} \
								} \
							TYPE(const TYPE& obj) = delete; \
						private:\
							TYPE(); \
							~TYPE(); \
							static TYPE* mInstance;

// Makes a classe into singleton which allows itself to be inherited.
// Don't forget to call Destory() to delete mObject!
#define SINGLETON_PROTECTED(TYPE) public:\
							static TYPE* GetInstance() \
								{ \
									if(mInstance == nullptr) \
										mInstance = DBG_NEW TYPE(); \
									return mInstance; \
								} \
							static void Destroy() \
								{ \
									delete mInstance; \
								} \
							TYPE(const TYPE& obj) = delete; \
						protected:\
							TYPE(); \
							~TYPE(); \
							static TYPE* mInstance;