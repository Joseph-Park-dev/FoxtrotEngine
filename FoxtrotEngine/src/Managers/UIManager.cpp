#include "Managers/UIManager.h"

#include <list>

#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Actors/UIs/UIActor.h"
#include "Managers/KeyInputManager.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
 // DEBUG

UIManager::UIManager()
	: mFocusedUI(nullptr)
{

}

UIManager::~UIManager()
{}

void UIManager::Update(float deltaTime)
{
	// 1. Identify Focused UI
	// 2. 
	mFocusedUI = GetFocusedUI();
	if (!mFocusedUI)
		return;
	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);
	bool lBtnAway = MOUSE_AWAY(MOUSE::MOUSE_LEFT);
	
	UIActor* targetUI = GetTargetedUI(mFocusedUI);
	if (targetUI != nullptr)
	{
		targetUI->OnMouseHovering();
		if (lBtnTap)
		{
			targetUI->OnMouseLButtonDown();
			targetUI->mLBtnDown = true;
		}
		else if (lBtnAway)
		{
			targetUI->OnMouseLButtonUp();
			if (targetUI->mLBtnDown)
			{
				targetUI->OnMouseLButtonClicked();
			}
			// Uncheck when the left button is released
			targetUI->mLBtnDown = false;
		}
	}
}

UIActor* UIManager::GetFocusedUI()
{
	Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
	std::vector<Actor*>& UI = currScene->GetActorGroup(ActorGroup::UI);

	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);

	UIActor* focusedUI = mFocusedUI;
	if (!lBtnTap)
		return focusedUI;

	std::vector<Actor*>::iterator targetIter= UI.end();
	std::vector<Actor*>::iterator iter = UI.begin();
	for (; iter != UI.end(); ++iter)
	{
		if (((UIActor*)*iter)->IsMouseHovering())
		{
			targetIter = iter;
		}
	}
	// There's no UI focused
	if (targetIter == UI.end())
	{
		return nullptr;
	}
	focusedUI = (UIActor*)*targetIter;
	UI.erase(targetIter);
	UI.push_back(focusedUI);
	return focusedUI;
}

UIActor* UIManager::GetTargetedUI(UIActor* parentUI)
{
	bool lBtnAway = MOUSE_AWAY(MOUSE::MOUSE_LEFT);

	UIActor* targetUI = nullptr;
	// 1. Including parent UI, inspect all its child objects
	//	  (using BFS utilizing queue DS)
	static std::list<UIActor*> queue;
	static std::vector<UIActor*> noneTarget;

	queue.clear();
	noneTarget.clear();
	
	queue.push_back(parentUI);

	while (!queue.empty())
	{
		// 2. Among the overlapped UIs, select the one with higher priority
		UIActor* UI = queue.front();
		queue.pop_front();

		if (UI->IsMouseHovering())
		{
			if (targetUI != nullptr)
			{
				noneTarget.push_back(targetUI);
			}
			targetUI = UI;
		}
		else
		{
			noneTarget.push_back(UI);
		}

		std::vector<Actor*>& childActors = UI->GetChildActors();
		for (size_t i = 0; i < childActors.size(); ++i)
		{
			queue.push_back((UIActor*)childActors[i]);
		}
		if (lBtnAway)
		{
			for (size_t i = 0; i < noneTarget.size(); ++i)
			{
				if (lBtnAway)
				{
					noneTarget[i]->mLBtnDown = false;
				}
			}
		}
	}
	return targetUI;
}

void UIManager::SetFocusedUI(UIActor* UI)
{
	if (mFocusedUI == UI || mFocusedUI == nullptr)
	{
		return;
	}
	mFocusedUI = UI;
	Scene* currScene = SceneManager::GetInstance()->GetCurrentScene();
	std::vector<Actor*>& UIObjs = currScene->GetActorGroup(ActorGroup::UI);
	std::vector<Actor*>::iterator iter = UIObjs.begin();
	for (; iter != UIObjs.end(); ++iter)
	{
		if (*iter == mFocusedUI)
		{
			break;
		}
	}
	UIObjs.erase(iter);
	UIObjs.push_back(mFocusedUI);
}

void UIManager::Reset()
{
	mFocusedUI = nullptr;
}

#ifdef FOXTROT_EDITOR

void UIManager::EditorUpdate(float deltaTime)
{
	mFocusedUI = EditorGetFocusedUI();
	if (!mFocusedUI)
		return;
	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);
	bool lBtnAway = MOUSE_AWAY(MOUSE::MOUSE_LEFT);
	UIActor* targetUI = GetTargetedUI(mFocusedUI);
	if (targetUI != nullptr)
	{
		targetUI->OnMouseHovering();
		if (lBtnTap)
		{
			targetUI->OnMouseLButtonDown();
			targetUI->mLBtnDown = true;
		}
		else if (lBtnAway)
		{
			targetUI->OnMouseLButtonUp();
			if (targetUI->mLBtnDown)
			{
				targetUI->OnMouseLButtonClicked();
			}
			// Uncheck when the left button is released
			targetUI->mLBtnDown = false;
		}
	}
}

UIActor* UIManager::EditorGetFocusedUI()
{
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::vector<Actor*>& elems = scene->GetActorGroup(ActorGroup::EDITOR);

	bool lBtnTap = MOUSE_TAP(MOUSE::MOUSE_LEFT);

	UIActor* focusedUI = mFocusedUI;
	if (!lBtnTap)
		return focusedUI;
	std::vector<Actor*>::iterator targetIter = elems.end();
	std::vector<Actor*>::iterator iter = elems.begin();
	for (; iter != elems.end(); ++iter)
	{
		EditorElement* element = (EditorElement*)(*iter);
		if (element->IsMouseHovering())
		{
			targetIter = iter;
		}
	}
	// There's no UI focused
	if (targetIter == elems.end())
	{
		return nullptr;
	}
	focusedUI = (EditorElement*)*targetIter;
	//elems.erase(targetIter);
	//elems.push_back(focusedUI);
	return focusedUI;
}
#endif