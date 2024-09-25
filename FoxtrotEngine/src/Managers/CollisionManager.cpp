#include "Managers/CollisionManager.h"

#include <iostream>

#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Components/ColliderComponent.h"
#include "Math/FTMath.h"
#include "Physics/Physics2D.h"
#include "Renderer/FoxtrotRenderer.h"


void CollisionManager::MarkGroup(ACTOR_GROUP left, ACTOR_GROUP right)
{
	UINT iRow = (UINT)left;
	UINT iCol = (UINT)right;
	if (iCol < iRow)
	{
		iRow = (UINT)right;
		iCol = (UINT)left;
	}
	if (mArrCollisionMarks[iRow] & (1 << iCol))
	{
		mArrCollisionMarks[iRow] &= ~(1 << iCol);
	}
	else
	{
		mArrCollisionMarks[iRow] |= (1 << iCol);
	}
}

void CollisionManager::RegisterRay(Physics::Ray* ray)
{
	mRegisteredRays.emplace_back(ray);
}

void CollisionManager::Reset()
{
	memset(mArrCollisionMarks, 0, sizeof(UINT) * (UINT)ACTOR_GROUP::END);
	ResetRegisteredRays();
	Physics2D::GetInstance()->ResetRayCasts();
	ResetIntersections();
}

void CollisionManager::Update()
{
	for (UINT iRow = 0; iRow < (UINT)ACTOR_GROUP::END; ++iRow)
	{
		for (UINT iCol = iRow; iCol < (UINT)ACTOR_GROUP::END; ++iCol)
		{
			if (mArrCollisionMarks[iRow] & (1 << iCol))
			{
				UpdateCollisionGroup((ACTOR_GROUP)iRow, (ACTOR_GROUP)iCol);
			}
		}
		CompareGroupWithRays((ACTOR_GROUP)iRow);
	}
}

void CollisionManager::UpdateCollisionGroup(ACTOR_GROUP left, ACTOR_GROUP right)
{
	Scene* pCurScene = SceneManager::GetInstance()->GetCurrScene();
	const std::vector<Actor*>& vecLeft = pCurScene->GetActorGroup(left);
	const std::vector<Actor*>& vecRight = pCurScene->GetActorGroup(right);
	for (size_t i = 0; i < vecLeft.size(); ++i)
	{
		ColliderComponent* vecLCollider = nullptr;
		ColliderComponent* vecRCollider = nullptr;
		if (vecLeft[i]->IsActive())
		{
			vecLCollider = vecLeft[i]->GetComponent<ColliderComponent>();
			if (!vecLCollider)
				continue;
			for (size_t j = 0; j < vecRight.size(); ++j)
			{
				if (vecRight[j]->IsActive())
				{
					vecRCollider = vecRight[j]->GetComponent<ColliderComponent>();
					if (!vecRCollider
						|| vecLeft[i] == vecRight[j])
						continue;
					COLLIDER_ID ID;
					ID.LeftID = vecLCollider->GetID();
					ID.RightID = vecRCollider->GetID();

					std::unordered_map<unsigned long long, bool>::iterator iter
						= mCollisionInfo.find(ID.ID);
					if (iter == mCollisionInfo.end())
					{
						mCollisionInfo.insert(std::make_pair(ID.ID, false));
						iter = mCollisionInfo.find(ID.ID);
					}

					if (HasCollided(vecLCollider, vecRCollider))
					{
						if (iter->second)
						{
							if (vecLeft[i]->IsDead() || vecRight[j]->IsDead())
							{
								vecLCollider->OnCollisionExit(vecRCollider);
								vecRCollider->OnCollisionExit(vecLCollider);
								iter->second = false;
							}
							else
							{
								vecLCollider->OnCollisionStay(vecRCollider);
								vecRCollider->OnCollisionStay(vecLCollider);
							}
						}
						else
						{
							if (!vecLeft[i]->IsDead() && !vecRight[j]->IsDead())
							{
								vecLCollider->OnCollisionEnter(vecRCollider);
								vecRCollider->OnCollisionEnter(vecLCollider);
								iter->second = true;
							}
						}
					}
					else
					{
						if (iter->second)
						{
							vecLCollider->OnCollisionExit(vecRCollider);
							vecRCollider->OnCollisionExit(vecLCollider);
							vecLCollider->mCollidedSide = CollidedSide::NONE;
							vecRCollider->mCollidedSide = CollidedSide::NONE;
							iter->second = false;
						}
					}
				}
			}
		}
	}
}

bool CollisionManager::HasCollided(ColliderComponent* leftCol, ColliderComponent* rightCol)
{
	if (AABBIntersection(leftCol, rightCol))
	{
		ConstainColliderPos(leftCol, rightCol);
		return true;
	}
	return false;
}

bool CollisionManager::AABBIntersection(ColliderComponent* aCol, ColliderComponent* bCol)
{
	FTVector2 aPos = aCol->GetFinalPosition();
	FTVector2 aScale = aCol->GetScale();

	FTVector2 bPos = bCol->GetFinalPosition();
	FTVector2 bScale = bCol->GetScale();

	float aColLeft   = aPos.x - aScale.x/2;
	float aColRight	 = aPos.x + aScale.x/2;
	float aColTop	 = aPos.y - aScale.y/2;
	float aColBottom = aPos.y + aScale.y/2;

	float bColLeft	 = bPos.x - bScale.x/2;
	float bColRight  = bPos.x + bScale.x/2;
	float bColTop	 = bPos.y - bScale.y/2;
	float bColBottom = bPos.y + bScale.y/2;

	bool aIsAboveB	 = aColBottom < bColTop;
	bool aIsBelowB	 = aColTop > bColBottom;
	bool aIsLeftToB	 = aColRight < bColLeft;
	bool aIsRightToB  = aColLeft > bColRight;

	return !(aIsAboveB || aIsBelowB || aIsLeftToB || aIsRightToB);
}

void CollisionManager::ConstainColliderPos(ColliderComponent* aCol, ColliderComponent* bCol)
{
	float scaleSumX = (aCol->GetScale().x + bCol->GetScale().x) / 2;
	float posDiffX = Math::Abs(aCol->GetFinalPosition().x - bCol->GetFinalPosition().x);
	float scaleSumY = (aCol->GetScale().y + bCol->GetScale().y) / 2;
	float posDiffY = Math::Abs(aCol->GetFinalPosition().y - bCol->GetFinalPosition().y);
	
	FTVector2 colliNormal = FTVector2::Zero;
	FTVector2 currPos = aCol->GetOwner()->GetTransform()->GetWorldPosition();
	
	float xOverLap = scaleSumX - posDiffX;
	float yOverLap = scaleSumY - posDiffY;
	if (yOverLap < xOverLap)
	{
		// aCol is above bCol
		if (aCol->GetFinalPosition().y < bCol->GetFinalPosition().y)
		{
			colliNormal = FTVector2(0, -1);
			aCol->mCollidedSide = CollidedSide::DOWN;
			bCol->mCollidedSide = CollidedSide::UP;
		}
		else
		{
			colliNormal = FTVector2(0, 1);
			aCol->mCollidedSide = CollidedSide::UP;
			bCol->mCollidedSide = CollidedSide::DOWN;
		}
		aCol->mFinalPosition += colliNormal * yOverLap;
		aCol->GetOwner()->GetTransform()->SetWorldPosition(currPos + colliNormal * yOverLap);
	}
	else
	{
		if (aCol->GetFinalPosition().x < bCol->GetFinalPosition().x)
		{
			colliNormal = FTVector2(-1, 0);
			aCol->mCollidedSide = CollidedSide::RIGHT;
			bCol->mCollidedSide = CollidedSide::LEFT;
		}
		else
		{
			colliNormal = FTVector2(1, 0);
			aCol->mCollidedSide = CollidedSide::LEFT;
			bCol->mCollidedSide = CollidedSide::RIGHT;
		}
		aCol->mFinalPosition += colliNormal * xOverLap;
		aCol->GetOwner()->GetTransform()->SetWorldPosition(currPos + colliNormal * xOverLap);
	}
}

void CollisionManager::CompareGroupWithRays(ACTOR_GROUP group)
{
	Scene* pCurScene = SceneManager::GetInstance()->GetCurrScene();
	const std::vector<Actor*>& actors = pCurScene->GetActorGroup(group);
	for (size_t i = 0; i < actors.size(); ++i)
	{
		ColliderComponent* collider = nullptr;
		if (actors[i]->IsActive())
		{
			collider = actors[i]->GetComponent<ColliderComponent>();
			if (collider != nullptr)
			{
				for (size_t i = 0; i < mRegisteredRays.size(); ++i)
				{
					if (HasRayCasted(mRegisteredRays[i], collider, group))
					{
						collider->OnRayEnter();
					}
				}
			}
		}
	}
}

void CollisionManager::RenderRay(FoxtrotRenderer* renderer)
{
	for (size_t i = 0; i < mRegisteredRays.size(); ++i)
	{
		/*FTVector2 ori = Camera2D::GetInstance()->ConvertScreenPosToWorld();
		FTVector2 end = Camera2D::GetInstance()->ConvertScreenPosToWorld();*/
		
		/*
		
			Alternative to 
			SDL_SetRenderDrawColor
				(renderer, 255, 255, 255, 255);

		*/
		FTVector2 ori = mRegisteredRays[i]->origin;
		FTVector2 end = mRegisteredRays[i]->endPoint;

		/*
			Alternative to
			SDL_RenderDrawLine(
				renderer,
				ori.x,
				ori.y,
				end.x,
				end.y
			);
		*/
	}
	Physics2D::GetInstance()->RenderRayCastHits(renderer);
	ResetRegisteredRays();
	Physics2D::GetInstance()->ResetRayCasts();
}

bool CollisionManager::HasRayCasted(Physics::Ray* ray, ColliderComponent* collider, ACTOR_GROUP group)
{
	if (ray->groupFilter == group)
	{
		std::wstring str = collider->GetOwner()->GetName();
		FTVector2 origin = ray->origin;
		FTVector2 endPoint = ray->endPoint;
		FTVector2 leftTop = collider->GetFinalPosition() - collider->mScale / 2;
		FTVector2 leftBottom =
			FTVector2(
				collider->GetFinalPosition().x - collider->mScale.x / 2,
				collider->GetFinalPosition().y + collider->mScale.y / 2
			);
		FTVector2 rightTop =
			FTVector2(
				collider->GetFinalPosition().x + collider->mScale.x / 2,
				collider->GetFinalPosition().y - collider->mScale.y / 2
			);
		FTVector2 rightBottom = collider->GetFinalPosition() + collider->mScale / 2;

		bool left	= RayIntersectedWithLine(origin, endPoint, leftTop,    leftBottom);
		bool right	= RayIntersectedWithLine(origin, endPoint, rightTop,   rightBottom);
		bool top	= RayIntersectedWithLine(origin, endPoint, leftTop,	   rightTop);
		bool bottom = RayIntersectedWithLine(origin, endPoint, leftBottom, rightBottom);

		if (left || right || top || bottom)
		{
			RayCastHit2D rayCastHit = {};
			rayCastHit.collider = collider;
			rayCastHit.actor = collider->GetOwner();
			rayCastHit.point = FindClosestIntersection(ray);
			rayCastHit.distance = (rayCastHit.point - ray->origin).Length();
			Physics2D::GetInstance()->RegisterRayCastHit(rayCastHit);

			return true;
		}
		return false;
	}
	return false;
}

bool CollisionManager::RayIntersectedWithLine(FTVector2 p1, FTVector2 p2, FTVector2 p3, FTVector2 p4)
{
	float uA = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) /
		((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
	float uB = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) /
		((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));

	if (0 <= uA && uA <= 1 && 0 <= uB && uB <= 1)
	{
		float intersectionX = p1.x + uA * (p2.x - p1.x);
		float intersectionY = p1.y + uA * (p2.y - p1.y);
		mIntersections.push_back(FTVector2(intersectionX, intersectionY));
		return true;
	}
	return false;
}

FTVector2 CollisionManager::FindClosestIntersection(Physics::Ray* ray)
{
	std::vector<FTVector2>::iterator iter = mIntersections.begin();
	FTVector2 closest = (*iter);
	for (; iter != mIntersections.end(); ++iter)
	{
		float distancePrev = (closest - ray->origin).LengthSq();
		float distance = ((*iter) - ray->origin).LengthSq();
		if (distance < distancePrev)
			closest = (*iter);
	}
	ResetIntersections();
	return closest;
}

void CollisionManager::ResetRegisteredRays()
{
	mRegisteredRays.clear();
	mRegisteredRays.shrink_to_fit();
}

void CollisionManager::ResetIntersections()
{
	mIntersections.clear();
	mIntersections.shrink_to_fit();
}

CollisionManager::CollisionManager()
{}

CollisionManager::~CollisionManager()
{}