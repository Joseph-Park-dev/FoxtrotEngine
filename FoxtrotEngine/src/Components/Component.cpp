#include "Component.h"

#include <nlohmann/json.hpp>

#include "Actors/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

void Component::ProcessInput(KeyInputManager* keyInputManager)
{
}

void Component::Update(float deltaTime)
{
}

void Component::LateUpdate(float deltaTime)
{
}

void Component::Render(FoxtrotRenderer* renderer)
{
}

Actor* Component::GetOwner() const
{
	return mOwner;
}

const int Component::GetUpdateOrder() const
{
	return mUpdateOrder;
}

const int Component::GetDrawOrder() const
{
	return mDrawOrder;
}

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
{
}

Component::~Component()
{
}

void Component::CloneTo(Actor* actor)
{
}

void Component::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::SaveInt(ofs, ChunkKeys::DRAW_ORDER, mDrawOrder);
	FileIOHelper::SaveInt(ofs, ChunkKeys::UPDATE_ORDER, mUpdateOrder);
}

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
	FileIOHelper::LoadInt(ifs, mDrawOrder);
}

#ifdef FOXTROT_EDITOR
void Component::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["Name"], GetName());
	FileIOHelper::AddScalarValue(out["DrawOrder"], mDrawOrder);
	FileIOHelper::AddScalarValue(out["UpdateOrder"], mUpdateOrder);
}

void Component::LoadProperties(nlohmann::ordered_json& in)
{
	mDrawOrder = FileIOHelper::LoadScalarValue<int>(in, "DrawOrder");
	mUpdateOrder = FileIOHelper::LoadScalarValue<int>(in, "UpdateOrder");
}

void Component::EditorUIUpdate()
{
}
void Component::RenderDebugGeometries(FoxtrotRenderer* renderer, ImDrawList* imDrawList, FTVector2 screenCenter)
{
}
#endif