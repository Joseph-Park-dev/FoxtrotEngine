#include "Camera2D.h"
#include "EditorCamera2D.h"
#include "CCore.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Tile.h"
#include "Transform.h"
#include "Random.h"

#include <math.h>

void Camera2D::Update(float deltaTime)
{
	UpdateTargetActor();
	if (mTargetActor)
	{
		if (mTargetActor->IsDead())
		{
			mTargetActor = nullptr;
		}
		else if (!mIsShaking)
		{
			mLookAtPos = mTargetActor->GetTransform()->GetWorldPosition();
		}
	}
	//UpdateEffect(deltaTime);
	UpdateShakeEffect(deltaTime);
	CalcDiff();
	CalcRenderArea();
}

void Camera2D::CalcDiff()
{
	mDiffFromCenter = mLookAtPos - mScreenCenter;
}

void Camera2D::CalcRenderArea()
{
	mRenderArea.x = mLookAtPos.x - mRenderResolution.x * 0.5f;
	mRenderArea.y = mLookAtPos.y - mRenderResolution.y * 0.5f;
	mRenderArea.w = mRenderResolution.x;
	mRenderArea.h = mRenderResolution.y;

	mEffectArea.x = mLookAtPos.x;
	mEffectArea.y = mLookAtPos.y;
	mEffectArea.w = mRenderResolution.x;
	mEffectArea.h = mRenderResolution.y;
}

void Camera2D::UpdateEffect(float deltaTime)
{
	if (mTargetActor)
	{
		mScreenCenter = mLookAtPos;
		ZoomIn();
	}

	if (mCamEffect.empty())
		return;
	CamEffect& effect = mCamEffect.front();
	// Start timer for each effects.
	effect.curTime += deltaTime;
	if (effect.effectDuration < effect.curTime)
	{
		mCamEffect.pop_front();
		return;
	}

	// Used for Fade in,out effect; 
	float ratio = 0.f;
	if (ratio < 0.f)
		ratio = 0;
	if (ratio > 1.f)
		ratio = 1;
	ratio = effect.curTime / effect.effectDuration;

	// Execution of each effects.
	if (effect.effect == CAM_EFFECT::NONE)
		return;
	else if (effect.effect == CAM_EFFECT::FADE_OUT)
	{
		mEffectAlpha = (int)(255.f * ratio);
	}
	else if (effect.effect == CAM_EFFECT::FADE_IN)
	{
		mEffectAlpha = (int)(255 * (1 - ratio));
	}
}

void Camera2D::UpdateShakeEffect(float deltaTime)
{
	if (mCamShakeEffect.empty())
	{
		mIsShaking = false;
		return;
	}
	mIsShaking = true;
	CamShakeEffect& effect = mCamShakeEffect.front();
	// Start timer for each effects.
	effect.curTime += deltaTime;
	if (effect.effectDuration < effect.curTime)
	{
		mCamShakeEffect.pop_front();
		mIsShaking = false;
		return;
	}

	// Used for tweening 
	float ratio = 0.f;
	if (ratio < 0.f)
		ratio = 0;
	if (ratio > 1.f)
		ratio = 1;
	ratio = effect.curTime / effect.effectDuration;

	if (effect.effect == CAM_EFFECT::SHAKE)
	{
		float x = Random::GetFloatRange(-effect.magnitude, effect.magnitude);
		float y = Random::GetFloatRange(-effect.magnitude, effect.magnitude);
		mLookAtPos += Vector2(x,y);
	}
}


void Camera2D::UpdateTargetActor()
{
	if (mTargetActor != nullptr)
	{
		if (mTargetActor->GetID() != mTargetActorID)
		{
			SetTarget(mTargetActorID);
			return;
		}
	}
	else
		SetTarget(mTargetActorID);
}

void Camera2D::SetTarget(int id)
{
	if (id == TARGET_NONE)
	{
		mTargetActor = nullptr;
		return;
	}
	for (size_t i = 0; i < (size_t)ACTOR_GROUP::END; ++i)
	{
		std::vector<Actor*>& actors = SceneManager::GetInstance()->GetCurrScene()->GetActorGroup(i);
		std::vector<Actor*>::iterator iter = actors.begin();
		for (; iter != actors.end(); ++iter)
		{
			if ((*iter)->GetID() == id)
			{
				mTargetActor = *iter;
				return;
			}
		}
	}
}

void Camera2D::ZoomIn()
{
	mZoomValue -= 0.01f;
}

void Camera2D::ZoomOut()
{
	mZoomValue += 0.01f;
}

void Camera2D::SetTargetActorID(int id)
{
	mTargetActorID = id;
}

bool Camera2D::IsInRenderedArea(SpriteComponent* sprite) const
{
	Vector2 screenPosLeftTop = Vector2(mRenderArea.x, mRenderArea.y);
	Vector2 screenPosRightBottom = Vector2(mRenderArea.x + mRenderArea.w, mRenderArea.y + mRenderArea.h);
	/*screenPosLeftTop = ConvertWorldPosToScreen(screenPosLeftTop);
	screenPosRightBottom = ConvertWorldPosToScreen(screenPosRightBottom);*/

	float screenLeft = screenPosLeftTop.x;
	float screenRight = screenPosRightBottom.x;
	float screenTop = screenPosLeftTop.y;
	float screenBottom = screenPosRightBottom.y;

	Bounds* rect = sprite->GetRect();
	int left = rect->x;
	int right = rect->x + rect->w;
	int top = rect->y;
	int bottom = rect->y + rect->h;

	return (screenLeft < right && left < screenRight)
		&& (screenTop < bottom && top < screenBottom);
}

// Tile's screen position is inside the camera area
bool Camera2D::IsInRenderedArea(Tile* tile) const
{
	Vector2 screenPosLeftTop = Vector2(mRenderArea.x, mRenderArea.y);
	Vector2 screenPosRightBottom = Vector2(mRenderArea.x + mRenderArea.w, mRenderArea.y + mRenderArea.h);
	/*screenPosLeftTop = ConvertWorldPosToScreen(screenPosLeftTop);
	screenPosRightBottom = ConvertWorldPosToScreen(screenPosRightBottom);*/

	float screenLeft = screenPosLeftTop.x;
	float screenRight = screenPosRightBottom.x;
	float screenTop = screenPosLeftTop.y;
	float screenBottom = screenPosRightBottom.y;

	int left = tile->GetPosOnScreen().x - tile->GetScale().x / 2;
	int right = tile->GetPosOnScreen().x + tile->GetScale().x / 2;
	int top = tile->GetPosOnScreen().y - tile->GetScale().y / 2;
	int bottom = tile->GetPosOnScreen().y + tile->GetScale().y / 2;

	return (screenLeft < right && left < screenRight)
		&& (screenTop < bottom && top < screenBottom);
}

Vector2 Camera2D::ConvertWorldPosToScreen(Vector2 worldPos) const
{
	Vector2 screenPos = worldPos - mDiffFromCenter;
	Vector2 zoomScale = (worldPos - mLookAtPos) * mZoomValue;
	return screenPos - zoomScale;
}

Vector2 Camera2D::ConvertScreenPosToWorld(Vector2 screenPos) const
{
	return screenPos + mDiffFromCenter;
}

void Camera2D::FadeIn(float duration)
{
	CamEffect eff = {};
	eff.effect = CAM_EFFECT::FADE_IN;
	eff.curTime = 0.f;
	if (duration == 0.f)
	{
		assert(nullptr);
	}
	eff.effectDuration = duration;
	mCamEffect.push_back(eff);
}

void Camera2D::FadeOut(float duration)
{
	CamEffect eff = {};
	eff.effect = CAM_EFFECT::FADE_OUT;
	eff.curTime = 0.f;
	if (duration == 0.f)
	{
		assert(nullptr);
	}
	eff.effectDuration = duration;
	mCamEffect.push_back(eff);
}

void Camera2D::Shake(float duration, float magnitude)
{
	if (!mIsShaking)
	{
		CamShakeEffect shakeEff = {};
		shakeEff.effect = CAM_EFFECT::SHAKE;
		shakeEff.curTime = 0.f;
		if (duration == 0.f)
			assert(nullptr);
		shakeEff.effectDuration = duration;
		shakeEff.magnitude = magnitude;
		mCamShakeEffect.push_back(shakeEff);
	}
}

#ifdef _DEBUG
//void Camera2D::Render(FoxtrotRenderer* renderer)
//{
//	SDL_SetRenderDrawColor
//	(
//		renderer,
//		0, 0, 0, mEffectAlpha
//	);
//	SDL_RenderFillRect(renderer, &mEffectArea);
//	//SDL_RenderDrawRect(CCore::GetInstance()->GetEditorRenderer(), &mEffectArea);
//}
#endif // _DEBUG

Camera2D::Camera2D()
	: mLookAtPos(Vector2::Zero)
	, mDiffFromCenter(Vector2::Zero)
	, mTargetActor(nullptr)
	, mTargetActorID(TARGET_NONE)
	, mScreenCenter(CCore::GetInstance()->GetResolution() / 2.f)
	, mRenderResolution(CCore::GetInstance()->GetResolution())
	, mRenderArea{}
	, mEffectArea{}
	, mEffectAlpha(0)
	, mIsShaking(false)
	, mZoomValue(0.f)
	, mZoomMultiplyValue(1.5f)
{
	CalcRenderArea();
}

Camera2D::~Camera2D()
{}