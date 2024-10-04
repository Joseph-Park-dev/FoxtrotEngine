#include "Component.h"

#include <nlohmann/json.hpp>

#include "Actors/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

void Component::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadInt(ifs, mDrawOrder);
	FileIOHelper::LoadInt(ifs, mUpdateOrder);
}

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

const Actor* Component::GetOwner() const
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

#ifdef FOXTROT_EDITOR
void Component::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["Name"], ToString(GetName()));
	FileIOHelper::AddScalarValue(out["DrawOrder"], mDrawOrder);
	FileIOHelper::AddScalarValue(out["UpdateOrder"], mUpdateOrder);
}
void Component::EditorUpdate(float deltaTime)
{
}
void Component::EditorLateUpdate(float deltaTime)
{
}
void Component::EditorRender(FoxtrotRenderer* renderer)
{
}
void Component::EditorUIUpdate()
{
}
#endif