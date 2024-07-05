#include "FoxtrotEngine/Actors/UIs/UIActor.h"

#include "FoxtrotEngine/Components/Component.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/RectTransform.h"
#include "FoxtrotEngine/Renderer/Camera2D.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/Actors/Actor.h"

void UIActor::UpdateActor(float deltaTime)
{
}

void UIActor::LateUpdateActor(float deltaTime)
{
	CheckMouseHover();
}

void UIActor::RenderActor(FoxtrotRenderer* renderer)
{
	//FTVector2 vPos = GetRectTransform()->GetAnchoredPosition();
	//FTVector2 vScale = GetRectTransform()->get();
	//if (mIsAffectedByCamera)
	//{
	//	vPos = Camera2D::GetInstance()->ConvertWorldPosToScreen(vPos);
	//}

	/*
	
		Alternative of
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_Rect rect = 
			{ 
				static_cast<int>(vPos.x - vScale.x * 0.5f * (1 - Camera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(vPos.y - vScale.x * 0.5f * (1 - Camera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(vScale.x * (1 - Camera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(vScale.y * (1 - Camera2D::GetInstance()->GetZoomValue()))
			};
			SDL_RenderDrawRect(renderer, &rect);
	
	*/
}

void UIActor::CheckMouseHover()
{
	FTVector2 mousePos = KeyInputManager::GetInstance()->GetMousePosition();
	if (mIsAffectedByCamera)
	{
		mousePos = Camera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}
	if (GetTransform()->GetWorldPosition().x - GetTransform()->GetScale().x/2 <= mousePos.x && 
		mousePos.x <= GetTransform()->GetWorldPosition().x + GetTransform()->GetScale().x/2 && 
		GetTransform()->GetWorldPosition().y - GetTransform()->GetScale().y/2 <= mousePos.y && 
		mousePos.y <= GetTransform()->GetWorldPosition().y + GetTransform()->GetScale().y/2)
	{
		mMouseHovering = true;
	}
	else
	{
		mMouseHovering = false;
	}
}

void UIActor::OnMouseHovering()
{}

void UIActor::OnMouseLButtonDown()
{
	printf("%ls", GetName().c_str());
}

void UIActor::OnMouseLButtonUp()
{
	//SDL_Log("%ls", GetName().c_str());
}

void UIActor::OnMouseLButtonClicked()
{
	//SDL_Log("%ls", GetName().c_str());
}

UIActor::UIActor(Scene* scene, bool isCamAffect)
	: Actor(scene)
	, mIsAffectedByCamera(isCamAffect)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mIsFocused(false)
{
	delete GetTransform();
	SetTransform(new RectTransform);
}

UIActor::UIActor(UIActor& origin)
	: Actor(origin)
	, mIsAffectedByCamera(origin.mIsAffectedByCamera)
	, mMouseHovering(origin.mMouseHovering)
	, mLBtnDown(origin.mLBtnDown)
	, mIsFocused(origin.mIsFocused)
{
}

UIActor::UIActor(Actor* origin)
	: Actor(origin)
	, mIsAffectedByCamera(true)
	, mMouseHovering(false)
	, mLBtnDown(false)
	, mIsFocused(false)
{
}

//UIActor::UIActor(const UIActor& origin)
//	: Actor(origin)
//	, mIsAffectedByCamera(origin.mIsAffectedByCamera)
//	, mMouseHovering(false)
//	, mLBtnDown(false)
//{
//}

UIActor::~UIActor()
{
}