#pragma once
#include <list>

#include "SingletonMacro.h"
#include "FTMath.h"
#include "Bounds.h"

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
	const FTVector2 GetLookAtPos() const { return mLookAtPos; }
	Bounds*			GetRenderArea() { return &mRenderArea; }
	FTVector2		GetScreenCenter() { return mScreenCenter; }
	int				GetTargetActorID() { return mTargetActorID; }
	FTVector2		GetRenderResolution() { return mRenderResolution; }
	FTVector2		GetDiffFromCenter() { return mDiffFromCenter; }
	float			GetZoomValue() { return mZoomValue; }

	void			SetLookAtPos(FTVector2 pos) { mLookAtPos = pos; }
	void			SetRenderResolution(FTVector2 res) { mRenderResolution = res; }
	void			SetTargetActorID(int id);
	void			SetScreenCenter(FTVector2 center) { mScreenCenter = center; }

	bool			IsInRenderedArea(SpriteComponent* sprite) const;
	bool			IsInRenderedArea(Tile* tile) const;

	virtual FTVector2 ConvertWorldPosToScreen(FTVector2 worldPos) const;
			FTVector2 ConvertScreenPosToWorld(FTVector2 screenPos) const;

	void			SetDiffFromCenter(FTVector2 diff) { mDiffFromCenter = diff; }

private:
	FTVector2	mLookAtPos;
	FTVector2	mDiffFromCenter;
	Actor*		mTargetActor;
	int			mTargetActorID;
	FTVector2	mScreenCenter;

	FTVector2	mRenderResolution;
	Bounds		mRenderArea;

	Bounds		mEffectArea;
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
	//void Render(FoxtrotRenderer* renderer);
#endif // _DEBUG

protected:
	virtual	void CalcDiff();
	virtual void CalcRenderArea();
			void ZoomIn();
			void ZoomOut();

private:
	void UpdateEffect(float deltaTime);
	void UpdateShakeEffect(float deltaTime);
	void UpdateTargetActor();
	void SetTarget(int id);
};