#pragma once
#include "Behaviors/FTBehavior.h"

class ButtonUI;

class ResponsiveButton :
	public FTBehavior
{
public:
	virtual std::string GetName() const override
	{
		return "ResponsiveButton";
	}

public:
	virtual void Setup() override;
	void		 Update(float deltaTime) override;
	void		 CloneTo(Actor* actor) override;

public:
	ResponsiveButton(Actor* owner, int updateOrder);
	~ResponsiveButton() override;

private:
	ButtonUI* mButton;
	float	  mCurrTime;
	float	  mDuration;
	float	  mRatio;

private:
	void OnMouseHover();
	void OnMouseEscape();
	void OnMouseClicked();

public:
	void SaveProperties(std::ofstream& ofs) override;
	void LoadProperties(std::ifstream& ifs) override;
};
