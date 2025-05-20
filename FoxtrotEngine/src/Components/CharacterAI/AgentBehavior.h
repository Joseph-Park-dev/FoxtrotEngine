#pragma once
#include "Components/Component.h"

#include "Actors/Transform.h"
#include "Components/CharacterAI/Steering.h"

class AgentBehavior :
	public Component
{
public:
	virtual Steering CreateSteering();

protected:
	Actor*		 GetTarget();
	FTDS::String& GetTargetName();

	void SetTarget(Actor* target);
	void SetTargetName(FTDS::String& targetName);

public:
	void Initialize(FTCore* ftCoreInstance) override;
	void Setup() override;
	void Update(float deltaTime) override;

public:
	AgentBehavior(Actor* owner, int updateOrder);
	~AgentBehavior() override;

private:
	Actor*		mTarget;
	FTDS::String mTargetName;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* TARGET_NAME = "Target Name";
}