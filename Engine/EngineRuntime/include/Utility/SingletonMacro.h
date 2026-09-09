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
// Don't forget to call Destroy() to delete mObject!
#define SINGLETON(TYPE)                 \
public:                                 \
	/** \
	 * @brief Obtains the singleton shared by this type. \
	 * @return Borrowed instance pointer, created lazily on first access. \
	 * @note Synchronize concurrent access and lifetime changes externally. \
	 */ \
	static TYPE* GetInstance()          \
	{                                   \
		if (mInstance == nullptr)       \
			mInstance = DBG_NEW TYPE(); \
		return mInstance;               \
	}                                   \
	/** \
	 * @brief Deletes the shared instance and resets its pointer to nullptr. \
	 * @note Invalidates all borrowed instance pointers; synchronize concurrent access externally. \
	 */ \
	static void Destroy()               \
	{                                   \
		if (mInstance)                  \
		{                               \
			delete mInstance;           \
			mInstance = nullptr;        \
		}                               \
	}                                   \
	/** \
	 * @brief Disables copying of singleton state. \
	 * @param obj Source instance; this constructor is deleted. \
	 */ \
	TYPE(const TYPE& obj) = delete;     \
                                        \
private:                                \
	/** \
	 * @brief Initializes the state of the instance managed by this singleton. \
	 */ \
	TYPE();                             \
	/** \
	 * @brief Releases the state owned by the singleton instance. \
	 */ \
	~TYPE();                            \
	inline static TYPE* mInstance = nullptr;

// Makes a classe into singleton which allows itself to be inherited.
// Don't forget to call Destory() to delete mObject!
#define SINGLETON_PROTECTED(TYPE)       \
public:                                 \
	/** \
	 * @brief Obtains the singleton shared by this type. \
	 * @return Borrowed instance pointer, created lazily on first access. \
	 * @note Synchronize concurrent access and lifetime changes externally. \
	 */ \
	static TYPE* GetInstance()          \
	{                                   \
		if (mInstance == nullptr)       \
			mInstance = DBG_NEW TYPE(); \
		return mInstance;               \
	}                                   \
	/** \
	 * @brief Deletes the shared instance and resets its pointer to nullptr. \
	 * @note Invalidates all borrowed instance pointers; synchronize concurrent access externally. \
	 */ \
	static void Destroy()               \
	{                                   \
		if (mInstance)                  \
		{                               \
			delete mInstance;           \
			mInstance = nullptr;        \
		}                               \
	}                                   \
	/** \
	 * @brief Disables copying of singleton state. \
	 * @param obj Source instance; this constructor is deleted. \
	 */ \
	TYPE(const TYPE& obj) = delete;     \
                                        \
protected:                              \
	/** \
	 * @brief Initializes the state of the instance managed by this singleton. \
	 */ \
	TYPE();                             \
	/** \
	 * @brief Releases the state owned by the singleton instance. \
	 */ \
	~TYPE();                            \
	inline static TYPE* mInstance = nullptr;

#define SINGLETON_TEST(EXPDEF, TYPE)                 \
public:                                              \
	/** \
	 * @brief Obtains the singleton shared by this type. \
	 * @return Borrowed instance pointer; nullptr until Initialize() is called. \
	 * @note Synchronize concurrent access and lifetime changes externally. \
	 */ \
	static EXPDEF TYPE* GetInstance()                \
	{                                                \
		return mInstance;                            \
	}                                                \
	/** \
	 * @brief Deletes the shared instance and resets its pointer to nullptr. \
	 * @note Invalidates all borrowed instance pointers; synchronize concurrent access externally. \
	 */ \
	static void Destroy()                            \
	{                                                \
		if (mInstance)                               \
		{                                            \
			delete mInstance;                        \
			mInstance = nullptr;                     \
		}                                            \
	}                                                \
	/** \
	 * @brief Creates the shared instance once and records its engine context. \
	 * @param base Borrowed engine context associated with the singleton. \
	 * @note Synchronize concurrent initialization and destruction externally. \
	 */ \
	static void Initialize(Core::FTCore* base)       \
	{                                                \
		if (mInstance == nullptr)                    \
		{                                            \
			mInstance = DBG_NEW TYPE();              \
			mBase	  = base;                        \
		}                                            \
	}                                                \
	/** \
	 * @brief Exposes the engine context recorded during singleton initialization. \
	 * @return Borrowed engine context, valid while the engine remains alive. \
	 */ \
	static Core::FTCore* GetBase()                   \
	{                                                \
		return mBase;                                \
	}                                                \
	/** \
	 * @brief Disables copying of singleton state. \
	 * @param obj Source instance; this constructor is deleted. \
	 */ \
	TYPE(const TYPE& obj) = delete;                  \
                                                     \
private:                                             \
	/** \
	 * @brief Initializes the state of the instance managed by this singleton. \
	 */ \
	TYPE();                                          \
	/** \
	 * @brief Releases the state owned by the singleton instance. \
	 */ \
	~TYPE() override;                                \
	inline static TYPE*			mInstance = nullptr; \
	inline static Core::FTCore* mBase	  = nullptr;
