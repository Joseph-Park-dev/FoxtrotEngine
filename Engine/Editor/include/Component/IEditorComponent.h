#pragma once
namespace Core
{
	class IRenderer;
	class ICamera;
} // namespace Core

namespace Editor
{
	template <typename COMP>
	class IEditorComponent
	{
	public:
		virtual void EditorUpdate(float deltaTime)									 = 0;
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) = 0;
		virtual void EditorUIUpdate()												 = 0;

	public:
		void SetComp(COMP comp) { mComp = comp; }

	public:
		IEditorComponent(COMP* comp)
			: mComp(comp) {}

	protected:
		COMP* GetComp() { return mComp; };

	private:
		COMP* mComp;
	};
} // namespace Editor