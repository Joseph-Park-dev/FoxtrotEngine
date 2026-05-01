#pragma once
#include "Plugin/IPlugin.h"

namespace Core
{
	class ICore : public Core::IPlugin
	{
	public:
		//////////////////////////////////
		////// Initialization Phase //////
		//////////////////////////////////

		virtual void Initialize() = 0;
		virtual void Setup()	  = 0;

		///////////////////////
		////// Game Loop //////
		///////////////////////

		virtual void ProcessInput()				 = 0;
		virtual void Update(float deltaTime)	 = 0;
		virtual void LateUpdate(float deltaTime) = 0;
		virtual void Render()					 = 0;
		virtual void ProcessEvent()				 = 0;

		///////////////////////////////
		////// Termination Phase //////
		///////////////////////////////

		virtual void ShutDown() = 0;
	};
} // namespace Core