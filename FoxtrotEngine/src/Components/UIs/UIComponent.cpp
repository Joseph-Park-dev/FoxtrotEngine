#include "Components/UIs/UIComponent.h"

#include "Components/Component.h"
#include "Actors/Transform.h"
#include "Actors/RectTransform.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Managers/KeyInputManager.h"
#include "Managers/UIManager.h"
#include "Math/FTMath.h"
#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
#include "DebugGeometries.h"
#include "ResourceSystem/FTShape.h"
#include "CommandHistory.h"
#endif // DEBUG

void UIComponent::CheckMouseHover()
{
	FTVector2 mousePos		= KeyInputManager::GetInstance()->GetMousePosition();
	Transform* transform	= GetOwner()->GetTransform();
	FTVector2 worldPosition = transform->GetWorldPosition() * Camera::GetInstance()->GetPixelsPerUnit();
	FTVector2 scale = mSize * FTVector2(transform->GetScale()) * Camera::GetInstance()->GetPixelsPerUnit();

	if (mIsAffectedByCamera)
	{
		//mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}
	if (worldPosition.x - scale.x/2 <= mousePos.x &&	// Mouse X boundaries
		mousePos.x <= worldPosition.x + scale.x/2 &&

		-worldPosition.y - scale.y/2 <= mousePos.y &&	// Mouse Y boundaries
		mousePos.y <= -worldPosition.y + scale.y/2)
	{
		mMouseHovering = true;
	}
	else
	{
		mMouseHovering = false;
	}
}

void UIComponent::OnMouseHovering()
{}

void UIComponent::OnMouseLButtonDown()
{
	printf("%s", GetName().c_str());
}

void UIComponent::OnMouseLButtonUp()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UIComponent::OnMouseLButtonClicked()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UIComponent::Initialize(FTCore* ftCoreInst)
{
	UIManager::GetInstance()->RegisterUI(this);
#ifdef FOXTROT_EDITOR
	mDebugRect = new FTRectangle;
	mDebugRect->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
#endif // DEBUG
}

void UIComponent::Update(float deltaTime)
{
}

void UIComponent::LateUpdate(float deltaTime)
{
	CheckMouseHover();
}

void UIComponent::Render(FoxtrotRenderer* renderer)
{
}

UIComponent::UIComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mIsAffectedByCamera(false)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mLBtnClicked(false)
	, mIsFocused(false)
	, mDebugRect(nullptr)
	, mSize(FTVector2(1.f,1.f))
{}

UIComponent::~UIComponent()
{}

#ifdef FOXTROT_EDITOR
void UIComponent::EditorUpdate(float deltaTime)
{
	CheckMouseHover();

	Transform* transform = GetOwner()->GetTransform();
	FTVector3 pos = transform->GetWorldPosition();
	pos = pos * FTVector3(1.f, -1.f, 1.f);
	FTVector3 rot = transform->GetRotation();
	FTVector3 scale = FTVector3(
		transform->GetScale().x * mSize.x,
		transform->GetScale().y * mSize.y,
		1.f
	);

	mDebugRect->Update(
		pos,
		rot,
		scale,
		Camera::GetInstance()
	);
}

void UIComponent::EditorUIUpdate()
{
	CommandHistory::GetInstance()->UpdateVector2Value("Size", mSize, FLOATMOD_SPEED);
}
#endif // FOXTROT_EDITOR