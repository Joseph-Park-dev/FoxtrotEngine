#pragma once
#include "Behaviors/FTBehavior.h"

class MonkeySpin :
	public FTBehavior
{
public:
	virtual std::string GetName() const override
	{
		return "Monkey Spin";
	}

public:
	virtual void Initialize(FTCore* coreInstance) override {};
	void Setup() override;
	void Update(float deltaTime) override;
	void CloneTo(Actor* actor) override;

public:
	MonkeySpin(Actor* owner, int updateOrder);
	void SaveProperties(std::ofstream& ofs) override;
	void LoadProperties(std::ifstream& ifs) override;
	void ExitGame();
};