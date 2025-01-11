#pragma once
#include "Components/CharacterAI/Seek.h"

class Pursue :
    public Seek
{
public:
    void Setup() override;

private:
    float mMaxPrediction;
    Actor* mTargetAux;
};

