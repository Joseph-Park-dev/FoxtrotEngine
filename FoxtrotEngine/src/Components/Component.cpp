#include "Component.h"

#include <nlohmann/json.hpp>

#include "Actors/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

const char* Component::TypeNames[NUM_COMPONENT_TYPES] =
{
	"AIComponent",
	"AnimatorComponent",
	"BGSpriteComponent",
	"ColliderComponent",
	"InputMoveComponent",
	"MoveComponent",
	"Rigidbody2DComponent",
	"SpriteRendererComponent",
	"TileMapComponent",
	"SpriteAnimComponent"
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

void Component::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["Name"], ToString(GetName()));
	FileIOHelper::AddScalarValue(out["DrawOrder"], mDrawOrder);
	FileIOHelper::AddScalarValue(out["UpdateOrder"], mUpdateOrder);
}

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mDrawOrder);
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
}
