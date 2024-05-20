#include <nlohmann/json.hpp>

#include "Component.h"
#include "Actor.h"
#include "ChunkLoader.h"
#include "TemplateFunctions.h"

const char* Component::TypeNames[NUM_COMPONENT_TYPES] =
{
	"AIComponent",
	"AnimatorComponent",
	"AnimSpriteComponent",
	"BGSpriteComponent",
	"ColliderComponent",
	"InputMoveComponent",
	"MoveComponent",
	"Rigidbody2DComponent",
	"SpriteComponent",
	"TileMapComponent",
};

Component::Component(Actor* owner, int drawOrder, int updateOrder)
	: mOwner(owner)
	, mDrawOrder(drawOrder)
	, mUpdateOrder(updateOrder)
{
	mOwner->AddComponent(this);
}

Component::Component(const Component& origin)
	: mOwner(origin.mOwner)
	, mUpdateOrder(origin.mUpdateOrder)
	, mDrawOrder(origin.mDrawOrder)
{}

Component::~Component()
{}

void Component::SaveProperties(std::ofstream& ofs)
{
	//FileIOHelper::AddString	(of, GetName());
	FileIOHelper::AddInt(ofs, GetDrawOrder());
	FileIOHelper::AddInt(ofs, GetUpdateOrder());
}

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mDrawOrder);
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
}
