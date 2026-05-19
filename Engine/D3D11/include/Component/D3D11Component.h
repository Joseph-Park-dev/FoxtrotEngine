#pragma once
#include "Component/IComponent.h"

#include "FTDS/Static/FTString.h"

namespace Core
{
	class IInputDevice;
	class IActor;
	class ICamera;
} // namespace Core

namespace D3D11
{
	class Camera;
	class D3D11Renderer;
	class D3D11Component :
		public Core::IComponent
	{
	public:
		virtual const char* GetName() const = 0;

	public:
		virtual void Initialize();
		virtual void Setup();
		virtual void ProcessInput(Core::IInputDevice* inputDevice)					= 0;
		virtual void Update(float deltaTime)										= 0;
		virtual void LateUpdate(float deltaTime)									= 0;
		virtual void Render(D3D11::D3D11Renderer* renderer, D3D11::Camera* camInst) = 0;
		virtual void CloneTo(Core::IActor* actor)									= 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		virtual Core::IActor* GetOwner() override;
		virtual const int	  GetUpdateOrder() override;
		virtual const bool	  GetIsInitialized() const override;
		virtual const bool	  GetIsSetup() const override;
		virtual const bool	  GetIsActive() const override;

		virtual void SetIsActive(bool isActive) override;

	public:
		D3D11Component(Core::IActor* owner, int updateOrder = Core::DefaultVal::UPDATE_ORDER);
		virtual ~D3D11Component();

	private:
		Core::IActor* mOwner;
		int			  mUpdateOrder;

	private:
		// This is turned as true as the Initialize(FTCore*) is executed.
		bool mIsInitialized;
		// This is turned as true as the Setup() is executed.
		bool mIsSetup;
		// The component is alive in the game loop
		// (During ProcessInput, Update, LateUpdate and Render)
		bool mIsActive;

#ifdef FOXTROT_EDITOR

	public:
		virtual void EditorUpdate(float deltaTime) override;
		virtual void EditorRender(D3D11::D3D11Renderer* renderer, Core::ICamera* camInst) = 0;
		virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11