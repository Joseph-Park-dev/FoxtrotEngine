#pragma once
#include <SDL2/SDL.h>
#include <list>

#include "SingletonMacro.h"
#include "Math.h"

class Actor;
class SpriteComponent;
class Tile;
struct RotOscilliation;
struct Oscilliation;
enum class ACTOR_GROUP;

enum class CAM_EFFECT
{
	NONE,
	FADE_IN,
	FADE_OUT,
	SHAKE
};

struct CamEffect
{
	CAM_EFFECT  effect;
	float		effectDuration;
	float		curTime;
};

struct CamShakeEffect : CamEffect
{
	float magnitude;
};

#define TARGET_NONE -1
class Camera2D
{
	SINGLETON_PROTECTED(Camera2D);

public:
	//Camera Effects
	void		  FadeIn(float duration);
	void		  FadeOut(float duration);
	void		  Shake(float duration, float magnitude);

public:
	const Vector2 GetLookAtPos() const { return mLookAtPos; }
	SDL_Rect*	  GetRenderArea() { return &mRenderArea; }
	Vector2		  GetScreenCenter() { return mScreenCenter; }
	int			  GetTargetActorID() { return mTargetActorID; }
	Vector2		  GetRenderResolution() { return mRenderResolution; }
	Vector2		  GetDiffFromCenter() { return mDiffFromCenter; }
	float		  GetZoomValue() { return mZoomValue; }

	void		  SetLookAtPos(Vector2 pos) { mLookAtPos = pos; }
	void		  SetRenderResolution(Vector2 res) { mRenderResolution = res; }
	void		  SetTargetActorID(int id);
	void		  SetScreenCenter(Vector2 center) { mScreenCenter = center; }

	bool		  IsInRenderedArea(SpriteComponent* sprite) const;
	bool		  IsInRenderedArea(Tile* tile) const;

	Vector2		  ConvertWorldPosToScreen(Vector2 worldPos) const;
	Vector2		  ConvertScreenPosToWorld(Vector2 screenPos) const;

	void		  SetDiffFromCenter(Vector2 diff) { mDiffFromCenter = diff; }

private:
	Vector2		mLookAtPos;
	Vector2		mDiffFromCenter;
	Actor*		mTargetActor;
	int			mTargetActorID;
	Vector2		mScreenCenter;

	Vector2		mRenderResolution;
	SDL_Rect	mRenderArea;

	SDL_Rect	mEffectArea;
	int			mEffectAlpha;
	std::list<CamEffect>	  mCamEffect;
	std::list<CamShakeEffect> mCamShakeEffect;
	bool		mIsShaking;
	float		mZoomValue;
	float		mZoomMultiplyValue;


public:
	void Init() {};
	virtual void Update(float deltaTime);

#ifdef _DEBUG
	void Render(SDL_Renderer* renderer);
#endif // _DEBUG

protected:
			void CalcDiff();
	virtual void CalcRenderArea();
			void ZoomIn();
			void ZoomOut();

private:
	void UpdateEffect(float deltaTime);
	void UpdateShakeEffect(float deltaTime);
	void UpdateTargetActor();
	void SetTarget(int id);
};